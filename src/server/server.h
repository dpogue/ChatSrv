#ifndef _SERVER_H_
#define _SERVER_H_

#include <list>
#include <set>
#include <vector>
#include <cstring>
#include <map>
#include <ctime>
#include <netdb.h>
#include "../user.h"
#include "../channel.h"

#define BUFLEN 512
#define LISTENQ 5

struct set_strcmp {
    bool operator()(const char* s1, const char* s2) {
        return strcasecmp(s1, s2) < 0;
    }
};

typedef struct _server {
    std::list<user*>* users;
    std::set<char*, set_strcmp>* nicknames;
    std::map<char*, channel*, set_strcmp>* channels;
    int fd_listen;
    short port;
    char* servname;
    char* version;
    time_t starttime;
    char* motdfile;
    std::vector<char*>* motdtext;
    FILE* log;
} server;

/**
 * Initialize a server structure to run on the given port.
 *
 * @author Darryl Pogue
 * @return Pointer to the initialized server structure.
 */
server* server_init();

/**
 * Closes the server's connections and frees the data associated with it.
 *
 * @author Darryl Pogue
 * @param srv The server to destroy.
 */
void server_destroy(server* srv);

/**
 * Accepts an incoming connection.
 *
 * @author Darryl Pogue
 * @author Terence Stenvold
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

/**
 * Parses a line of received data for commands.
 *
 * @author Darryl Pogue
 * @param srv The pointer to the server structure.
 * @param sender The sender of the message.
 * @param cmd The command that was sent.
 */
void parse_cmd(server* srv, user* sender, char* cmd);

#endif
