#include "connection.h"
#include "server.h"
#include "messages.h"
#include <netdb.h>
#include <sys/socket.h>

void get_host_name(user* u) {
    hostent* hp;

    send_message(u, "NOTICE AUTH :*** Looking up your hostname...\n");

    hp = gethostbyaddr((char*)&(u->addr.sin_addr.s_addr),
            sizeof(u->addr.sin_addr.s_addr), AF_INET);
    u->host = hp;

    send_message(u, "NOTICE AUTH :*** Found your hostname\n");
}
