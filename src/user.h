#ifndef _USER_H_
#define _USER_H_

#include <cstdio>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

typedef struct _user {
    char* nickname;
    int socket;
    sockaddr_in hostname;
} user;

user* accept_user(int fd, sockaddr_in addr);

void disconnect_user(user* usr);

#endif
