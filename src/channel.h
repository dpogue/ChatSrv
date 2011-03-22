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

/**
 * Destroys a channel and frees all the memory associated with it.
 *
 * @author Darryl Pogue
 * @param chan The channel to destroy.
 */
void destroy_channel(channel* chan);

/**
 * Sends a message to all users of a channel, optionally skipping the
 * user who originally sent the message.
 *
 * @author Darryl Pogue
 * @param chan The channel to which the message is being sent.
 * @param msg The message to send.
 * @param sender The user to skip, which can be NULL to send to all users.
 */
void send_to_channel(channel* chan, char* msg, struct _user* sender);

/**
 * Joins a user to a channel.
 *
 * @author Darryl Pogue
 * @param chan The channel that is being joined.
 * @param user The user who is joining the channel.
 */
void join_channel(channel* chan, struct _user* user);

/**
 * Gets the list of names of all users in a channel.
 *
 * @author Darryl Pogue
 * @param chan The channel for which the users are being fetched.
 * @param num_msgs A pointer that holds the number of NAMES replies after
 *                  this function is called.
 * @return An array of string messages for a NAMES response.
 */
char** get_channel_names(channel* chan, int* num_msgs);

/**
 * Removes a user from a channel, sending the specified message to all other
 * users in the channel.
 *
 * @author Darryl Pogue
 * @param chan The channel to leave.
 * @param user The user who is leaving.
 * @param msg The parting message to be sent to other users.
 * @return true if the channel is now empty (and has been deleted).
 */
bool leave_channel(channel* chan, struct _user* user, char* msg);

/**
 * Sets the topic of a channel.
 *
 * @author Darryl Pogue
 * @param chan The channel in which to set the topic.
 * @param topic The topic line.
 * @param who The user who set the topic.
 */
void channel_set_topic(channel* chan, char* topic, struct _user* who);

#endif
