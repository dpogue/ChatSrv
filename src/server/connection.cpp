#include "connection.h"
#include "messages.h"
#include <netdb.h>
#include <sys/socket.h>
#include <vector>
#include <cstring>
#include <cctype>

using namespace std;

void get_host_name(user* u) {
    hostent* hp;

    char* msg = noticemsg_s("AUTH", "*** Looking up your hostname...");
    send_message(u, msg);
    free(msg);

    hp = gethostbyaddr((char*)&(u->addr.sin_addr.s_addr),
            sizeof(u->addr.sin_addr.s_addr), AF_INET);
    if (hp != NULL) {
        u->host = hp;

        u->hostname = hp->h_name;
    } else {
        herror("gethostbyaddr");
        u->hostname = "unknown-host";
    }

    msg = noticemsg_s("AUTH", "*** Found your hostname");
    send_message(u, msg);
    free(msg);
}

void send_motd(server* srv, user* u) {
    if (srv->motdtext == NULL) {
        FILE* fd = fopen(srv->motdfile, "r");
        if (fd == NULL) {
            char* msg = ":MOTD file is missing";
            msg = numericmsg(srv, u, 422, msg);
            send_message(u, msg);
            free(msg);
            return;
        }

        srv->motdtext = new vector<char*>();

        char line[80];
        while (fgets(line, sizeof(line), fd) != NULL) {
            while (isspace(line[strlen(line) - 1])) {
                line[strlen(line) - 1] = '\0';
            }
            srv->motdtext->push_back(strdup(line));
        }
        fclose(fd);
    }

    char* tmp;
    char* msg = (char*)malloc(512);
    sprintf(msg, ":- %s Message of the Day -", srv->servname);
    tmp = numericmsg(srv, u, 375, msg);
    send_message(u, tmp);
    free(tmp);

    for (vector<char*>::iterator it = srv->motdtext->begin();
            it != srv->motdtext->end(); ++it)
    {
        sprintf(msg, ":- %s", *it);
        tmp = numericmsg(srv, u, 372, msg);
        send_message(u, tmp);
        free(tmp);
    }

    sprintf(msg, ":End of the /MOTD command.");
    tmp = numericmsg(srv, u, 376, msg);
    send_message(u, tmp);
    free(tmp);
    free(msg);
}

void send_welcome_info(server* srv, user* u) {
    char* msg = (char*)malloc(512);
    char* uhost = user_userhost_name(u);
    sprintf(msg, ":Welcome to the IRC Network %s", uhost);
    free(uhost);
    char* sndmsg = numericmsg(srv, u, 1, msg);
    send_message(u, sndmsg);
    free(sndmsg);

    sprintf(msg, ":Your host is %s, running version %s",
            srv->servname, srv->version);
    sndmsg = numericmsg(srv, u, 2, msg);
    send_message(u, sndmsg);
    free(sndmsg);

    sprintf(msg, "*** Your host is %s, running version %s",
            srv->servname, srv->version);
    sndmsg = noticemsg_s(srv->servname, msg);
    send_message(u, sndmsg);
    free(sndmsg);

    sprintf(msg, ":This server was created %s", ctime(&srv->starttime));
    sndmsg = numericmsg(srv, u, 3, msg);
    send_message(u, sndmsg);
    free(sndmsg);
    free(msg);
}
