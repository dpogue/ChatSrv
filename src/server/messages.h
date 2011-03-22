#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#include <cstdio>
#include <cstring>
#include <unistd.h>
#include "../user.h"
#include "../channel.h"
#include "server.h"

/**
 * Sends a message to a user.
 *
 * @author Darryl Pogue
 * @param client The user to which the message will be sent.
 * @param msg The message content.
 */
void send_message(user* client, char* msg);

/**
 * Creates a numeric server response message.
 *
 * @author Darryl Pogue
 * @param srv The server structure.
 * @param to The user to which this message will be sent.
 * @param id The numeric ID of the message.
 * @param msg The message content.
 * @return A formatted numeric server response.
 */
char* numericmsg(server* srv, user* to, int id, char* msg);

/**
 * Creates a pong reply.
 *
 * @author Darryl Pogue
 * @param srv The server structure.
 * @return A formatted PONG message.
 */
char* pongmsg(server* srv);

/**
 * Creates a join message for the specified user and channel.
 *
 * @author Darryl Pogue
 * @param sender The user from which the message originated.
 * @param chan The channel that is being joined.
 * @return A formatted JOIN message.
 */
char* joinmsg(user* sender, channel* chan);

/**
 * Creates a topic response message for the specified channel.
 *
 * @author Darryl Pogue
 * @param chan The channel for which the topic is being returned.
 * @return A formatted TOPIC message.
 */
char* topicmsg(channel* chan);

/**
 * Creates a notice message between two users.
 *
 * @author Darryl Pogue
 * @param sender The user from which the message originated.
 * @param receiver The user to which the message is being sent.
 * @param msg The message content.
 * @return A formatted NOTICE message.
 */
char* noticemsg(user* sender, char* receiver, char* msg);

/**
 * Creates a notice message from the server.
 *
 * @author Darryl Pogue
 * @param sender The pseudo-service from which the message originated.
 * @param msg The message content.
 * @return A formatted NOTICE message.
 */
char* noticemsg_s(char* sender, char* msg);

/**
 * Creates a private message between a user and another entity.
 *
 * @author Darryl Pogue
 * @param sender The user from which the message originated.
 * @param receiver The receiver of the message.
 * @param msg The message content.
 * @return A formatted PRIVMSG message.
 */
char* privmsg(user* sender, char* receiver, char* msg);

/**
 * Creates a nick message from a user with the specified nickname.
 *
 * @author Darryl Pogue
 * @param sender The user who is sending the message.
 * @param name The new nickname.
 * @return A formatted NICK message.
 */
char* nickmsg(user* sender, char* name);

/**
 * Creates a part message from a channel for a user with the specified text.
 *
 * @author Darryl Pogue
 * @param sender The user who is sending the message.
 * @param chan The channel that is being left.
 * @param text The textual leaving reason.
 * @return A formatted PART message.
 */
char* partmsg(user* sender, channel* chan, char* msg);

/**
 * Creates a quit message from a user with the specified text.
 *
 * @author Darryl Pogue
 * @param sender The user who is sending the message.
 * @param text The textual quit reason.
 * @return A formatted QUIT message.
 */
char* quitmsg(user* sender, char* text);

#endif
