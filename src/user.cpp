#include "user.h"
#include "channel.h"
#include <unistd.h>

using namespace std;

user* accept_user(int fd, sockaddr_in addr) {
    user* u = (user*)malloc(sizeof(user));
    u->nickname = NULL;
    u->realname = NULL;
    u->username = NULL;
    u->socket = fd;
    u->addr = addr;
    u->hostname = NULL;
    u->away = false;
    u->channels = new list<channel*>();
    
    return u;
}

char* user_userhost(user* usr) {
    char* name = (char*)malloc(128);

    sprintf(name, "~%s@%s", usr->username, usr->hostname);

    return name;
}

char* user_userhost_name(user* usr) {
    char* name = (char*)malloc(128);

    sprintf(name, "%s!~%s@%s", usr->nickname, usr->username, usr->hostname);

    return name;
}

void destroy_user(user* usr) {
    free(usr->nickname);
    free(usr->username);
    free(usr->realname);

    delete usr->channels;

    close(usr->socket);
}
