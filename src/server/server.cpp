#include "server.h"
#include "connection.h"
#include "messages.h"
#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <queue>

using namespace std;

server* server_init(short port) {
    server* srv = (server*)malloc(sizeof(server));
    int fd = -1;
    int arg = 1;
    sockaddr_in addr;

    srv->servname = (char*)malloc(MAXHOSTNAMELEN);
    gethostname(srv->servname, MAXHOSTNAMELEN);

    srv->port = port;
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
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(fd, (sockaddr*)&addr, sizeof(addr)) == -1) {
        /* Bind the address to the socket.
         * Exit is unsuccessful. */
        exit(1);
    }

    srv->fd_listen = fd;
    srv->users = new list<user*>();

    fprintf(stderr, "Users = %d\n", srv->users->size());

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

    for (list<user*>::iterator i = srv->users->begin();
            i != srv->users->end(); ++i) {
        /* Broadcast the join message to all other users before adding
           ourselves to the list */

        send_message(*i, "Client Joined\n");
    }

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

    while ((nread = read(sender->socket, bp, bsize)) > 0) {
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

        for (list<user*>::iterator i = srv->users->begin();
                i != srv->users->end(); ++i) {
            /* Broadcast the quit message to all other users */
            send_message(*i, "Client Left\n");
        }
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

            char* outbuf = (char*)malloc(512);
            sprintf(outbuf, "[%s] %s",
                    inet_ntoa(sender->addr.sin_addr), buf);

            for (list<user*>::iterator i = srv->users->begin();
                    i != srv->users->end(); ++i) {
                /* Broadcast the message to all other users */
                if (*i == sender) {
                    continue;
                }

                send_message(*i, outbuf);
            }
        }
    }
}

void parse_cmd(server* srv, user* sender, char* cmd) {
    char* token = NULL;

    token = strtok(cmd, " ");

    if (!strcmp(token, "NICK")) {
        char* nick = strtok(NULL, " \r");

        if (sender->nickname != NULL) {
            free(sender->nickname);
        }

        sender->nickname = strdup(nick);
    } else if (!strcmp(token, "USER")) {
        char* uname = strtok(NULL, " ");
        char* hname = strtok(NULL, " ");
        char* sname = strtok(NULL, " ");
        char* rname = strtok(NULL, "\n");
        rname++; /* Ignore the leading colon */

        sender->username = strdup(uname);
        sender->hostname = strdup(hname);
        sender->servname = strdup(sname);
        sender->realname = strdup(rname);

        fprintf(stderr, "%s@%s is %s\n", uname, sname, rname);
        
        char* welcome = numericmsg(srv, sender, 1, "Welcome to the server!");
        send_message(sender, welcome);
    } else {
    }
}
