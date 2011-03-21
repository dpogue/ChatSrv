#include "channel.h"
#include "user.h"
#include "server/messages.h"
#include <list>
#include <cstdio>

using namespace std;

channel* create_channel(server* server, char* name) {
    channel* chan = (channel*)malloc(sizeof(channel));
    chan->topic = NULL;
    chan->topic_who = NULL;
    chan->topic_time = 0;
    chan->server = server;

    chan->name = strdup(name);

    chan->users = new list<channel_user>();

    return chan;
}

void destroy_channel(channel* chan) {
    free(chan->name);
    free(chan->topic);
    free(chan->topic_who);

    delete chan->users;

    free(chan);
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
    } else {
        c_user.mode = ' ';
    }

    chan->users->push_back(c_user);
    user->channels->push_back(chan);

    char* msg = joinmsg(user, chan);
    send_to_channel(chan, msg, NULL);
    free(msg);

    if (chan->topic == NULL) {
        msg = numericmsg(chan->server, user, 331, ":No topic is set");
        send_message(user, msg);
        free(msg);
    } else {
        char* topic = (char*)malloc(512);
        sprintf(topic, "%s :%s", chan->name, chan->topic);
        msg = numericmsg(chan->server, user, 332, topic);
        send_message(user, msg);
        free(msg);
        free(topic);

        topic = (char*)malloc(512);
        sprintf(topic, "%s %s %d", chan->name, chan->topic_who,
                chan->topic_time);
        msg = numericmsg(chan->server, user, 333, topic);
        send_message(user, msg);
        free(msg);
        free(topic);
    }

    int len = 0;
    char** names = get_channel_names(chan, &len);

    for (int i = 0; i < len; i++) {
        msg = numericmsg(chan->server, user, 353, names[i]);
        send_message(user, msg);
        free(names[i]);
        free(msg);
    }
    free(names);

    char* tmp = (char*)malloc(512);
    sprintf(tmp, "%s :End of /NAMES list", chan->name);
    msg = numericmsg(chan->server, user, 366, tmp);
    send_message(user, msg);
    free(msg);
    free(tmp);
}

char** get_channel_names(channel* chan, int* num_msgs) {
    char** ret = (char**)malloc(16 * sizeof(char*));
    char* msg = (char*)malloc(512);
    sprintf(msg, "= %s :", chan->name);

    for (list<channel_user>::iterator it = chan->users->begin();
            it != chan->users->end(); ++it)
    {
        if (strlen(msg) + strlen(it->user->nickname) + 2 < 512) {
            char* tmp = strdup(msg);
            if (it->mode != ' ') {
                sprintf(msg, "%s%c%s ", tmp, it->mode, it->user->nickname); 
            } else {
                sprintf(msg, "%s%s ", tmp, it->user->nickname); 
            }
            free(tmp);
        } else {
            ret[(*num_msgs)++] = msg;

            msg = (char*)malloc(512);
            sprintf(msg, "= %s :", chan->name);
        }
    }

    ret[(*num_msgs)++] = msg;

    return ret;
}

void leave_channel(channel* chan, user* user, char* msg) {
    list<channel_user>::iterator idx = chan->users->end();

    for (list<channel_user>::iterator it = chan->users->begin();
            it != chan->users->end(); ++it) {
        if (it->user == user) {
            idx = it;
        } else {
            send_message(it->user, msg);
        }
    }

    if (idx != chan->users->end()) {
        chan->users->erase(idx);
    }
}

void channel_set_topic(channel* chan, char* topic, user* who) {
    for (list<channel_user>::iterator it = chan->users->begin();
            it != chan->users->end(); ++it) {
        char mode = it->mode;
        if (it->user == who &&
                mode != '@' && mode != '~' && mode != '&' && mode != '%')
        {
            char* tmp = (char*)malloc(512);
            sprintf(tmp, "%s :%s", chan->name, "You're not a channel operator");
            char* msg = numericmsg(chan->server, who, 482, tmp);
            send_message(who, msg);
            free(msg);
            free(tmp);
            return;
        }
    }

    chan->topic = strdup(topic);
    chan->topic_who = strdup(who->nickname);
    chan->topic_time = time(NULL);

    for (list<channel_user>::iterator it = chan->users->begin();
            it != chan->users->end(); ++it) {
        char* msg = topicmsg(chan);
        send_message(it->user, msg);
        free(msg);
    }
}
