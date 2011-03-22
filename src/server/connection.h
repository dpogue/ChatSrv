#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include "../user.h"
#include "server.h"

/**
 * Looks up the proper hostname for a user via reverse DNS.
 *
 * @author Darryl Pogue
 * @param u The user whose hostname will be queried.
 */
void get_host_name(user* u);

/**
 * Sends the Message Of The Day to the specified user.
 *
 * @author Darryl Pogue
 * @param srv The server structure.
 * @param u The user to which the messages will be sent.
 */
void send_motd(server* srv, user* u);

/**
 * Sends the server welcome information to the specified user.
 *
 * @author Darryl Pogue
 * @param srv The server structure.
 * @param u The user to which the messages will be sent.
 */
void send_welcome_info(server* srv, user* u);

#endif
