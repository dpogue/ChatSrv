#include "messages.h"

void send_message(user* client, char* msg) {
    write(client->socket, msg, strlen(msg));
}

char* numericmsg(server* srv, user* to, int id, char* msg) {
    char* num = (char*)malloc(512);
    char* nick = "*";

    if (to->nickname != NULL) {
        nick = to->nickname;
    }

    sprintf(num, ":%s %03d %s :%s\n", srv->servname, id, nick, msg);

    return num;
}

char* pongmsg(server* srv) {
    char* pong = (char*)malloc(512);

    sprintf(pong, ":%s PONG %s :%s\n", srv->servname, srv->servname, srv->servname);

    return pong;
}

char* joinmsg(user* sender, char* receiver) {
    char* join = (char*)malloc(512);

    sprintf(join, ":%s JOIN %s\n", sender->nickname, receiver);

    return join;
}

char* noticemsg(user* sender, char* receiver, char* msg) {
    char* notice = (char*)malloc(512);

    sprintf(notice, ":%s NOTICE %s :%s\n", sender->nickname, receiver, msg);

    return notice;
}

char* noticemsg_s(char* sender, char* msg) {
    char* notice = (char*)malloc(512);

    sprintf(notice, "NOTICE %s :%s\n", sender, msg);

    return notice;
}

char* privmsg(user* sender, char* receiver, char* msg) {
    char* priv = (char*)malloc(512);

    sprintf(priv, ":%s PRIVMSG %s :%s\n", sender->nickname, receiver, msg);

    return priv;
}

char* partmsg(user* sender) {
    char* part = (char*)malloc(512);

    sprintf(part, ":%s PART\n", sender->nickname);

    return part;
}
