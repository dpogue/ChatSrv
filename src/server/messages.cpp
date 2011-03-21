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

    sprintf(num, ":%s %03d %s %s\n", srv->servname, id, nick, msg);

    return num;
}

char* pongmsg(server* srv) {
    char* pong = (char*)malloc(512);

    sprintf(pong, ":%s PONG %s :%s\n", srv->servname, srv->servname, srv->servname);

    return pong;
}

char* joinmsg(user* sender, channel* chan) {
    char* join = (char*)malloc(512);
    char* uhost = user_userhost_name(sender);

    sprintf(join, ":%s JOIN %s\n", uhost, chan->name);

    free(uhost);
    return join;
}

char* topicmsg(channel* chan) {
    char* topic = (char*)malloc(512);

    sprintf(topic, ":%s TOPIC %s :%s\n", chan->topic_who, chan->name, chan->topic);

    return topic;
}

char* noticemsg(user* sender, char* receiver, char* msg) {
    char* notice = (char*)malloc(512);
    char* uhost = user_userhost_name(sender);

    sprintf(notice, ":%s NOTICE %s :%s\n", uhost, receiver, msg);

    free(uhost);
    return notice;
}

char* noticemsg_s(char* sender, char* msg) {
    char* notice = (char*)malloc(512);

    sprintf(notice, "NOTICE %s :%s\n", sender, msg);

    return notice;
}

char* privmsg(user* sender, char* receiver, char* msg) {
    char* priv = (char*)malloc(512);
    char* uhost = user_userhost_name(sender);

    sprintf(priv, ":%s PRIVMSG %s :%s\n", uhost, receiver, msg);

    free(uhost);
    return priv;
}

char* nickmsg(user* sender, char* name) {
    char* nick = (char*)malloc(512);
    char* uhost = user_userhost_name(sender);

    sprintf(nick, ":%s NICK :%s\n", uhost, name);

    free(uhost);
    return nick;
}

char* partmsg(user* sender, channel* chan, char* msg) {
    char* part = (char*)malloc(512);
    char* uhost = user_userhost_name(sender);

    sprintf(part, ":%s PART %s :%s\n", uhost, chan->name, msg);

    free(uhost);
    return part;
}

char* quitmsg(user* sender, char* text) {
    char* quit = (char*)malloc(512);
    char* uhost = user_userhost_name(sender);

    sprintf(quit, ":%s QUIT :%s\n", uhost, text);

    free(uhost);
    return quit;
}
