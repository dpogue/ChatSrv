#include "server.h"

int main() {
    server* srv = server_init();

    server_loop(srv);

    return 0;
}
