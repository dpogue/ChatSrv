#include "server.h"
#include "connection.h"
#include "messages.h"
#include "../3rdParty/inih/ini.h"
#include <cstdio>
#include <cstring>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <queue>

using namespace std;

static int confighandler(void*, const char*, const char*, const char*);

server* server_init() {
    server* srv = (server*)malloc(sizeof(server));
    int fd = -1;
    int arg = 1;
    sockaddr_in addr;

    if (ini_parse("/etc/dodo.ini", &confighandler, srv) < 0) {
        fprintf(stderr, "Could not load /etc/dodo.ini file\n");
        exit(1);
    }

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        /* Create the socket.
         * Exit if unsuccessful. */
        exit(1);
    }

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &arg, sizeof(arg)) == -1) {
        /* Allow the socket to rebind to the port.
         * Exit if unsuccessful. */
        exit(1);
    }

    /* Set up the socket address. Accept connections from any client. */
    bzero((char *)&addr, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(srv->port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(fd, (sockaddr*)&addr, sizeof(addr)) == -1) {
        /* Bind the address to the socket.
         * Exit is unsuccessful. */
        exit(1);
    }

    srv->fd_listen = fd;
    srv->users = new list<user*>();
    srv->nicknames = new set<char*, set_strcmp>();
    srv->channels = new map<char*, channel*, set_strcmp>();

    srv->starttime = time(NULL);

    return srv;
}

void server_accept(server* srv) {
    sockaddr_in addr;
    socklen_t len = sizeof(addr);
    user* u;

    int client = accept(srv->fd_listen, (sockaddr*)&addr, &len);
    if (client == -1) {
        herror("server_accept: accept");
        exit(1);
    }
    fprintf(stdout, "Connection from %s\n", inet_ntoa(addr.sin_addr));

    u = accept_user(client, addr);
    get_host_name(u);
    srv->users->push_back(u);
}

void server_loop(server* srv) {
    int fd = srv->fd_listen;
    int maxsock = fd;
    fd_set socks;

    listen(fd, LISTENQ);

    while (true) {
        FD_ZERO(&socks);
        FD_SET(fd, &socks);
        for (list<user*>::iterator i = srv->users->begin();
                i != srv->users->end(); ++i) {
            /* Loop over the connected clients to build the fd set */
            FD_SET((*i)->socket, &socks);
            if (maxsock < (*i)->socket) {
                maxsock = (*i)->socket;
            }
        }

        select(maxsock+1, &socks, NULL, NULL, NULL);

        if (FD_ISSET(fd, &socks)) {
            /* Incoming connection, need to accept it */
            server_accept(srv);
        }

        for (list<user*>::iterator i = srv->users->begin();
                i != srv->users->end(); ++i) {
            if (FD_ISSET((*i)->socket, &socks)) {
                server_read_msg(srv, *i);
                break;
            }
        }
    }
}

void server_read_msg(server* srv, user* sender) {
    int nread = 0;
    char* buf = (char*)malloc(BUFLEN);
    char* bp = buf;
    int bsize = BUFLEN;

    while (bsize && (nread = read(sender->socket, bp, bsize)) > 0) {
        bp += nread;
        bsize -= nread;

        if (*(bp - 1) == '\r' || *(bp - 1) == '\n' || bsize == 0) {
            *bp = '\0';
            break;
        }
    }

    if (nread == 0) {
        /* The client has disconnected */
        srv->users->remove(sender);
        srv->nicknames->erase(sender->nickname);

        char* qmsg = quitmsg(sender, "Client Exited.");
        for (list<channel*>::iterator it = sender->channels->begin();
                it != sender->channels->end(); ++it) {
            leave_channel(*it, sender, qmsg);
        }
        free(qmsg);
    } else if (nread > 0) {
        std::queue<char*> lines;

        char* line = strtok(buf, "\n\r");
        while (line != NULL) {
            lines.push(strdup(line));
            line = strtok(NULL, "\n\r");
        }

        while (lines.size()) {
            line = lines.front();
            lines.pop();
            parse_cmd(srv, sender, line);
        }
    }
}

void parse_cmd(server* srv, user* sender, char* cmd) {
    char* token = NULL;

    token = strtok(cmd, " ");

    if (!strcmp(token, "NICK")) {
        char* nick = strtok(NULL, " \n");

        fprintf(stderr, "Got nickname request %s\n", nick);

        if (srv->nicknames->find(nick) != srv->nicknames->end()) {
            fprintf(stderr, "\tFound in set!\n");
            char* msg = (char*)malloc(512);
            sprintf(msg, "%s :Nickname already in use", nick);
            char* tmp = numericmsg(srv, sender, 433, msg);
            send_message(sender, tmp);
            free(tmp);
            free(msg);

            return;
        }

        if (sender->nickname != NULL) {
            char* msg = nickmsg(sender, nick);
            send_message(sender, msg);

            for (list<channel*>::iterator it = sender->channels->begin();
                    it != sender->channels->end(); ++it) {
                send_to_channel(*it, msg, sender);
            }

            free(msg);

            srv->nicknames->erase(sender->nickname);
            free(sender->nickname);
        }

        if (sender->nickname == NULL && sender->username != NULL) {
            sender->nickname = strdup(nick);
            send_welcome_info(srv, sender);

            fprintf(stderr, "%s@%s is %s\n",
                    sender->username, sender->hostname, sender->realname);

            send_motd(srv, sender);
        } else {
            sender->nickname = strdup(nick);
        }

        srv->nicknames->insert(sender->nickname);
    } else if (!strcmp(token, "USER")) {
        char* uname = strtok(NULL, " ");
        strtok(NULL, ":");
        char* rname = strtok(NULL, "\n");

        sender->username = strdup(uname);
        sender->realname = strdup(rname);

        if (sender->nickname != NULL) {
            send_welcome_info(srv, sender);

            fprintf(stderr, "%s@%s is %s\n", uname, sender->hostname, rname);

            send_motd(srv, sender);
        }
    } else if (!strcmp(token, "QUIT")) {
        char* text = strtok(NULL, " ");
        text = strpbrk(text, ":");
        ++text;

        char* msg = quitmsg(sender, text);
        for (list<channel*>::iterator it = sender->channels->begin();
                it != sender->channels->end(); ++it) {
            leave_channel(*it, sender, msg);
        }
        free(msg);

        srv->nicknames->erase(sender->nickname);
        srv->users->remove(sender);
        destroy_user(sender);
    } else if (!strcmp(token, "JOIN")) {
        char* chan_name = strtok(NULL, " \n");
        channel* chan = NULL;
        map<char*, channel*>::iterator it = srv->channels->find(chan_name);

        if (*chan_name != '#' && *chan_name != '&' && *chan_name != '~') {
            char* tmp = (char*)malloc(512);
            sprintf(tmp, "%s :No such channel", chan_name);
            char* msg = numericmsg(srv, sender, 403, tmp);
            send_message(sender, msg);
            free(msg);
            free(tmp);
            return;
        }

        if (it == srv->channels->end()) {
            chan = create_channel(srv, chan_name);
            srv->channels->insert(make_pair(chan_name, chan));
        } else {
            chan = it->second;
        }

        join_channel(chan, sender);
    } else if (!strcmp(token, "PING")) {
        char* msg = pongmsg(srv);
        send_message(sender, msg);
    } else if (!strcmp(token, "PRIVMSG")) {
        char* who = strtok(NULL, " ");
        char* line = strtok(NULL, "\n");
        line = strpbrk(line, ":");
        ++line;

        if (*who == '#' || *who == '&' || *who == '~') {
            /* Sending to a channel */

            char* msg = NULL;
            map<char*, channel*>::iterator it = srv->channels->find(who);
            if (it == srv->channels->end()) {
                char* tmp = (char*)malloc(512);
                sprintf(tmp, "%s :No such channel", who);
                msg = numericmsg(srv, sender, 403, tmp);
                send_message(sender, msg);
                free(msg);
                free(tmp);
                return;
            }

            msg = privmsg(sender, who, line);
            send_to_channel(it->second, msg, sender);
            free(msg);
            return;
        }

        char* msg = privmsg(sender, who,  line);
        for (list<user*>::iterator i = srv->users->begin();
                i != srv->users->end(); ++i) {
            if (!strcmp((*i)->nickname, who)) {
                send_message(*i, msg);
                free(msg);
                return;
            }
        }
        free(msg);
    } else if (!strcmp(token, "NOTICE")) {
        char* who = strtok(NULL, " ");
        char* line = strtok(NULL, "\n");
        line = strpbrk(line, ":");
        ++line;

        char* msg = noticemsg(sender, who,  line);
        for (list<user*>::iterator i = srv->users->begin();
                i != srv->users->end(); ++i) {
            if (!strcmp((*i)->nickname, who)) {
                send_message(*i, msg);
                free(msg);
                return;
            }
        }
        free(msg);
    } else if (!strcmp(token, "USERHOST")) {
        char* who = strtok(NULL, "\n");

        for (list<user*>::iterator i = srv->users->begin();
                i != srv->users->end(); ++i) {
            if (!strcmp((*i)->nickname, who)) {
                char* msg = (char*)malloc(512);
                sprintf(msg, ":%s=%c%s", (*i)->nickname,
                        ((*i)->away ? '-' : '+'), user_userhost(*i));
                char* tmp = numericmsg(srv, sender, 302, msg);
                send_message(sender, tmp);
                free(tmp);
                free(msg);
            }
        }
    }
}

static int confighandler(void* user, const char* section, const char* name,
                        const char* value) {
    server* srv = (server*)user;

    #define MATCH(s, n) strcasecmp(section, s) == 0 && strcasecmp(name, n) == 0
    if (MATCH("server", "hostname")) {
        srv->servname = strdup(value);
    } else if (MATCH("server", "version")) {
        srv->version = strdup(value);
    } else if (MATCH("server", "port")) {
        srv->port = atoi(value);
    } else if (MATCH("server", "motd")) {
        srv->motdfile = strdup(value);
    }

    return 0;
}
