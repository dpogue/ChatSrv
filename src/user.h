#ifndef _USER_H_
#define _USER_H_

#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct _user {
    char* nickname;
    char* username;
    char* hostname;
    char* servname;
    char* realname;
    int socket;
    sockaddr_in addr;
    hostent* host;
    bool away;
} user;

user* accept_user(int fd, sockaddr_in addr);

/**
 * Returns the userhost of the specified user.
 * The userhost is in the format ~[username]@[hostname].
 *
 * @author Darryl Pogue
 * @param usr The user to query.
 * @return The userhost information of the user.
 */
char* user_userhost(user* usr);

/**
 * Returns the userhost with the nickname of the specified user.
 * The userhost is in the format [nickname]!~[username]@[hostname].
 *
 * @author Darryl Pogue
 * @param usr The user to query.
 * @return The userhost and nickname information of the user.
 */
char* user_userhost_name(user* usr);

void disconnect_user(user* usr);

#endif
