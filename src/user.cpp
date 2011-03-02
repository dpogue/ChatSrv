#include "user.h"

user* accept_user(int fd, sockaddr_in addr) {
    user* u = (user*)malloc(sizeof(user));
    u->nickname = NULL;
    u->socket = fd;
    
    hostent* host = gethostbyaddr(inet_ntoa(addr.sin_addr));
    if (host == NULL) {
        herror("accept_user(): gethostbyaddr");
        return NULL;
    }
    u->hostname = host;

    return u;
}
