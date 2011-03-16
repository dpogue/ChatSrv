#include "channel.h"
#include "user.h"
#include "server/messages.h"
#include <list>
#include <cstdio>

using namespace std;

channel* create_channel(char* name) {
    channel* chan = (channel*)malloc(sizeof(channel));
    chan->topic = NULL;
    chan->topic_who = NULL;
    chan->topic_time = 0;

    chan->name = strdup(name);

    chan->users = new list<channel_user>();

    return chan;
}

void send_to_channel(channel* chan, char* msg, user* sender) {
    for (list<channel_user>::iterator it = chan->users->begin();
            it != chan->users->end(); ++it) {
        if (it->user != sender) {
            send_message(it->user, msg);
        }
    }
}

void join_channel(channel* chan, user* user) {
    channel_user c_user;
    c_user.user = user;

    if (chan->users->size() == 0) {
        c_user.mode = '@';
    }

    chan->users->push_back(c_user);

    char* msg = joinmsg(user, chan);
    send_to_channel(chan, msg, NULL);
    free(msg);
}

void channel_set_topic(channel* chan, char* topic, user* who) {
    chan->topic = strdup(topic);
    chan->topic_who = strdup(who->nickname);
    chan->topic_time = time(NULL);
}
