#ifndef _MAIN_
#define _MAIN_

#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_TCP_PORT 7000
#define BUFLEN 1024
#define TRUE 1
#define LISTENQ 5


/**
 * Shows a system error and exits
 *
 * @author Richard Stevens / Aman Abdulla / Terence Stenvold 
 * @param message is the error message received
 */ 
static void SystemFatal(const char* message);


/**
 * Sends a buffer to all connected clients except the one
 * who send the data
 *
 * @author Terence Stenvold
 * @param buf is the buffer to send
 * @param sender is the index of the sender in the client array
 * @param client array containing sockets of connected clients
 * @param maxi is the number of currently connected clients
 */
void sendToAll(char *buf, int sender, int *client, int maxi);

/**
 * Returns the port number specified. Defaults to 7000
 *
 * @author Terence Stenvold
 * @param argc number of command line args
 * @param argv the char *'s of the arguments 
 */
int parseArgs(int argc, char **argv);

#endif
