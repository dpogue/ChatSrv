#include "connection.h"
#include "messages.h"
#include <netdb.h>
#include <sys/socket.h>
#include <vector>

using namespace std;

void get_host_name(user* u) {
    hostent* hp;

    char* msg = noticemsg_s("AUTH", "*** Looking up your hostname...");
    send_message(u, msg);
    free(msg);

    hp = gethostbyaddr((char*)&(u->addr.sin_addr.s_addr),
            sizeof(u->addr.sin_addr.s_addr), AF_INET);
    u->host = hp;

    msg = noticemsg_s("AUTH", "*** Found your hostname");
    send_message(u, msg);
    free(msg);
}

void send_motd(server* srv, user* u) {
    static vector<char*> motd;

    if (motd.size() == 0) {
        FILE* fd = fopen(srv->motdfile, "r");
        if (fd == NULL) {
            char* msg = "MOTD file is missing";
            msg = numericmsg(srv, u, 422, msg);
            send_message(u, msg);
            free(msg);
            return;
        }

        char line[80];
        while (fgets(line, sizeof(line), fd) != NULL) {
            motd.push_back(strdup(line));
        }
        fclose(fd);
    }

    char* tmp;
    char* msg = (char*)malloc(512);
    sprintf(msg, "- %s Message of the Day -", srv->servname);
    tmp = numericmsg(srv, u, 375, msg);
    send_message(u, tmp);
    free(tmp);

    for (vector<char*>::iterator it = motd.begin();
            it != motd.end(); ++it)
    {
        sprintf(msg, "- %s", *it);
        tmp = numericmsg(srv, u, 372, msg);
        send_message(u, tmp);
        free(tmp);
    }

    sprintf(msg, "End of the /MOTD command.");
    tmp = numericmsg(srv, u, 376, msg);
    send_message(u, tmp);
    free(tmp);
    free(msg);
}

void send_welcome_info(server* srv, user* u) {
    char* msg = numericmsg(srv, u, 1, "Welcome to the server!");
    send_message(u, msg);

    sprintf(msg, "Your host is %s, running version %s",
            srv->servname, srv->version);
    char* sndmsg = numericmsg(srv, u, 2, msg);
    send_message(u, sndmsg);
    free(sndmsg);

    sndmsg = noticemsg_s(srv->servname, msg);
    send_message(u, sndmsg);
    free(sndmsg);

    sprintf(msg, "This server was created %s", ctime(&srv->starttime));
    char* tmp = numericmsg(srv, u, 3, msg);
    send_message(u, tmp);
    free(tmp);
    free(msg);
}
