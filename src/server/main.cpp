#include "server.h"

int main() {
    server* srv = server_init(PORT);

    server_loop(srv);

    return 0;
}
