#ifndef _SERVER_H_
#define _SERVER_H_

#include <list>
#include <ctime>
#include <netdb.h>
#include "../user.h"

#define BUFLEN 512
#define LISTENQ 5

typedef struct _server {
    std::list<user*>* users;
    int fd_listen;
    short port;
    char* servname;
    char* version;
    time_t starttime;
    char* motdfile;
} server;


/**
 * Initialize a server structure to run on the given port.
 *
 * @author Darryl Pogue
 * @return Pointer to the initialized server structure.
 */
server* server_init();

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
