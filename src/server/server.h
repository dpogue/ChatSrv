#ifndef _SERVER_H_
#define _SERVER_H_

#include <list>
#include <netdb.h>
#include "../user.h"

#define PORT 6667
#define BUFLEN 512
#define LISTENQ 5
#define MAXHOSTNAMELEN 64

typedef struct _server {
    std::list<user*>* users;
    int fd_listen;
    short port;
    char* servname;
} server;


/**
 * Initialize a server structure to run on the given port.
 *
 * @author Darryl Pogue
 * @param port The port on which to run the server.
 * @return Pointer to the initialized server structure.
 */
server* server_init(short port);

/**
 * Accepts an incoming connection.
 *
 * @author Darryl Pogue
 * @param srv The pointer to the server structure.
 */
void server_accept(server* srv);

/**
 * The main networking loop.
 *
 * @author Darryl Pogue
 * @author Terence Stenvold
 * @param srv The pointer to the server structure.
 */
void server_loop(server* srv);

/**
 * Reads a message from a connected socket.
 *
 * @author Darryl Pogue
 * @author Terence Stenvold
 * @param srv The pointer to the server structure.
 * @param sender The sender of the message.
 */
void server_read_msg(server* srv, user* sender);

void parse_cmd(server* srv, user* sender, char* cmd);

#endif
