/**
 * Simple Client for CommAudio
 * 
 * @date March 19th 2010
 * @version 0.1
 * @author Terence Stenvold
 * 
 */

#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>


#define SERVER_TCP_PORT 6667// Default port
#define BUFLEN 4096  // Buffer length

void writetoFile(FILE *fp, char *msg, pthread_mutex_t *mtx);
void openFile(FILE **fp, char *filename);
void conServer(int* sd, int port, char* host);
void initChat(char* nick, char* channel, char* server, int sd);
void recvLoop(int sd, FILE *fp, int argc, pthread_mutex_t *mtx);



/**
 * Main Function for the client
 * Takes arguments and starts the clien
 *
 * @param argc is the number of arguments gived
 * @param argv is the actual arguments
 * @author Terence Stenvold
 */
int main(int argc, char** argv)
{
    int sd, port, child;
    char* host;
    char sbuf[BUFLEN], tbuf[BUFLEN];
    char nick[BUFLEN];
    char channel[BUFLEN];
    pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
    FILE *fp = NULL;
    
    switch (argc) {
        case 2:
            host = argv[1]; // Host name
            port = SERVER_TCP_PORT;
            break;
        case 3:
        case 4:
            host = argv[1];
            port = atoi(argv[2]); // User specified port
            break;
        default:
            fprintf(stderr, "Usage: %s host [port] [filename] \n", argv[0]);
            exit(1);
    }

    if(argc == 4) {
	openFile(&fp,argv[3]);
    }

    conServer(&sd, port, host);

    if((child = fork()) > 0) {
        recvLoop(sd,fp,argc,&mtx);
    }

    initChat(nick, channel, host, sd);

    while (1) {
        fgets(tbuf, BUFLEN, stdin);
        
	if(strlen(tbuf) == 3 && strcmp(tbuf,"/q\n") == 0 ) {
	    printf("\n\nQUITTING!\n");
	    break;
	}
	//Build the message to send
	sprintf(sbuf, "PRIVMSG %s :%s", channel, tbuf);
	
 	if(argc == 4) {
	    writetoFile(fp,tbuf,&mtx);
	}
	send(sd, sbuf, strlen(sbuf), 0);
    }

    kill(child, SIGKILL);
    close(sd);
    fclose(fp);
    return 0;
}


/**
 * writes a message to the file
 * locks the mutex to prevent both writing to it at
 * the same tim
 *
 * @param fp is the file pointer
 * @param msg is the message to write
 * @param mtx is the mutex
 * @author Terence Stenvold
 */
void writetoFile(FILE *fp, char *msg, pthread_mutex_t *mtx)
{
    pthread_mutex_unlock(mtx);
    fprintf(fp,"%s",msg);
    pthread_mutex_unlock(mtx);
    fflush(fp);
}



/**
 * Opens a log file
 *
 * @param fp is the file pointer
 * @param filename is the file to open
 * @author Terence Stenvold
 */
void openFile(FILE **fp, char *filename) 
{
    *fp = fopen (filename,"w");
    if(*fp==NULL)
    {
	printf("file open failed\n");
	exit(1);
    }
}


/**
 * Connects to the server specified 
 * and creates a socket descriptor
 *
 * @param sd is the socket descriptor
 * @param port is kinda obvious
 * @param host also self explainitory
 * @author Terence Stenvold
 */
void conServer(int* sd, int port, char* host)
{
    struct hostent* hp;
    struct sockaddr_in server;

    // Create the socket
    if ((*sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Cannot create socket");
        exit(1);
    }

    bzero((char*)&server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if ((hp = gethostbyname(host)) == NULL) {
        fprintf(stderr, "Unknown server address\n");
        exit(1);
    }

    bcopy(hp->h_addr, (char*)&server.sin_addr, hp->h_length);

    // Connecting to the server
    if (connect(*sd, (struct sockaddr*)&server, sizeof(server)) == -1) {
        fprintf(stderr, "Can't connect to server\n");
        perror("connect");
        exit(1);
    }
}


/**
 * Loops and recvies from the socket
 * and the prints to the screen
 * 
 * @param sd is the socket descriptor
 * @param fp is the file pointer
 * @param argc number of arguments passed
 * @param mtx is the mutex
 * @author Terence Stenvold
 */
void recvLoop(int sd, FILE *fp, int argc, pthread_mutex_t *mtx)
{
    int n, bytes_to_read;
    char rbuf[BUFLEN];
    char tbuf[8];
    char* bp;

    while(1) {
        
	n = 0;
        bp = rbuf;
        bytes_to_read = BUFLEN;

        while ((n = recv(sd, bp, bytes_to_read, 0)) < BUFLEN) {
            bp += n;
            bytes_to_read -= n;

	    //READ until buffer full or \n recieved
            if (*(bp - 1) == '\n') {
                *bp = '\0';
                break;
            }

            usleep(100);
        }

        strncpy(tbuf, rbuf, 6);

	//Ignore NOTICE messages
        if (strcmp(tbuf, "NOTICE") != 0) {
	    if(argc == 4) {
		writetoFile(fp,rbuf,mtx);
	    }
	    printf("%s",rbuf);
            fflush(stdout);
        }
    }

    exit(0);
}


/**
 * Initailizes the chat with the 
 * proper sequence of messages from 
 * the user
 *
 * @param nick is a char array for a nickanme
 * @param channel is the channel to join
 * @param server is the hostname 
 * @param sd is the socket descriptor 
 * @author Terence Stenvold
 */
void initChat(char* nick, char* channel, char* server, int sd)
{
    char tbuf[BUFLEN];
    printf("Nickname: ");
    fgets(nick, BUFLEN, stdin);
    printf("Channel: ");
    fgets(channel, BUFLEN, stdin);
    sprintf(tbuf, "NICK %s", nick);
    send(sd, tbuf, strlen(tbuf), 0);
    sprintf(tbuf, "USER fakeuser fakeuser %s :Fake Name\n", server);
    send(sd, tbuf, strlen(tbuf), 0);
    sprintf(tbuf, "JOIN %s", channel);
    send(sd, tbuf, strlen(tbuf), 0);
    //remove the \n 
    channel[strlen(channel) - 1] = '\0';
}
