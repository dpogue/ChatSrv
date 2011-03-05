#include "user.h"

user* accept_user(int fd, sockaddr_in addr) {
    user* u = (user*)malloc(sizeof(user));
    u->nickname = NULL;
    u->socket = fd;
    u->hostname = addr;
    
    return u;
}
