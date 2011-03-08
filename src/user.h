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
} user;

user* accept_user(int fd, sockaddr_in addr);

void disconnect_user(user* usr);

#endif
