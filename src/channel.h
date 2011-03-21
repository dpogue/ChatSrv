#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <list>
#include <cstring>

enum usermode {
    kFounder    = '~',
    kAdmin      = '&',
    kOperator   = '@',
    kHalfOp     = '%',
    kVoice      = '+',
};

typedef struct _channel_user {
    struct _user* user;
    char mode;
} channel_user;

typedef struct _channel {
    char* name;
    char* topic;
    char* topic_who;
    unsigned int topic_time;
    struct _server* server;
    std::list<channel_user>* users;
} channel;

channel* create_channel(struct _server* server, char* name);

void destroy_channel(channel* chan);

void send_to_channel(channel* chan, char* msg, struct _user* sender);

void join_channel(channel* chan, struct _user* user);

char** get_channel_names(channel* chan, int* num_msgs);

void leave_channel(channel* chan, struct _user* user, char* msg);

void channel_set_topic(channel* chan, char* topic, struct _user* who);

#endif
