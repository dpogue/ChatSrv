#include "messages.h"

void send_message(user* client, char* msg) {
    write(client->socket, msg, strlen(msg));
}

char* joinmsg(user* sender, char* receiver) {
    char* join = (char*)malloc(512);

    sprintf(join, ":%s JOIN %s\n", sender->nickname, receiver);

    return join;
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
