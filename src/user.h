#ifndef _USER_H_
#define _USER_H_

#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <list>

typedef struct _user {
    char* nickname;
    char* username;
    char* hostname;
    char* realname;
    int socket;
    sockaddr_in addr;
    hostent* host;
    std::list<struct _channel*>* channels;
    bool away;
} user;

/**
 * Creates a new user for a newly accepted socket.
 *
 * @author Darryl Pogue
 * @param fd The file descriptor of the socket.
 * @param addr The socket address.
 * @return A new user structure.
 */
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

/**
 * Closes the user's connections and frees the data associated with
 * the user.
 *
 * @author Darryl Pogue
 * @param usr The user to destroy.
 */
void destroy_user(user* usr);

#endif
