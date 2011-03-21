#include "server.h"
#include <signal.h>
#include <cstdio>

server* srv = NULL;

/**
 * Action handler to shut down the server when SIGINT is received.
 *
 * @author Darryl Pogue
 * @param sig The signal number that was received.
 */
void sigint_handler(int sig);

/** Application Entry Point */
int main() {
    struct sigaction sa_int;

    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
        perror("sigaction failed");
        exit(1);
    }

    /* Initialize and run the server */
    srv = server_init();

    server_loop(srv);

    return 0;
}

void sigint_handler(int sig) {

    if (sig != SIGINT) {
        return;
    }

    printf("Shutting down the server\n");
    server_destroy(srv);
    exit(0);
}
