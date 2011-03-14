#ifndef _CHANNEL_H_
#define _CHANNEL_H_

enum usermode {
    kOperator   = (1 << 0),
    kVoice      = (1 << 1)
};

typedef struct _channel_user {
    struct _user* user;
    char mode;
} channel_user;

typedef struct _channel {
    char* name;
    char* topic;
    std::list<channel_user>* users;
} channel;

//char* chars_for_mode(usermode mode);

channel* create_channel(char* name);

void join_channel(channel* chan, user* user);

void channel_set_topic(channel* chan, char* topic);

#endif
