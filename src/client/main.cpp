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

#define SERVER_TCP_PORT 6667// Default port
#define BUFLEN 4096  // Buffer length

void conServer(int *sd, int port, char *host);
void initChat(char *nick, char *channel, char *server, int sd);
void recvLoop(int sd);

int main (int argc, char **argv)
{
    int sd, port;
    char *host;
    char *bp; 
    char sbuf[BUFLEN], tbuf[BUFLEN];
    char nick[BUFLEN];
    char channel[BUFLEN];

    switch(argc)
    {
    case 2:
	host =argv[1];// Host name
	port =SERVER_TCP_PORT;
	break;
    case 3:
	host =argv[1];
	port =atoi(argv[2]);// User specified port
	break;
    default:
	fprintf(stderr, "Usage: %s host [port]\n", argv[0]);
	exit(1);
    }

    conServer(&sd, port, host);

    if(fork() > 0) {
	recvLoop(sd);	
    }

    initChat(nick,channel,host,sd);
    while (1) {
	fgets(tbuf, BUFLEN, stdin);
        sprintf(sbuf, "PRIVMSG %s :%s", channel, tbuf);
	send(sd, sbuf, strlen(sbuf), 0);
    }

    close (sd);
    return 0;
}


void conServer(int *sd, int port, char *host) 
{
    struct hostent *hp;
    struct sockaddr_in server;

    // Create the socket
    if ((*sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
	perror("Cannot create socket");
	exit(1);
    }
    bzero((char *)&server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if ((hp = gethostbyname(host)) == NULL)
    {
	fprintf(stderr, "Unknown server address\n");
	exit(1);
    }
    bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);

    // Connecting to the server
    if (connect (*sd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
	fprintf(stderr, "Can't connect to server\n");
	perror("connect");
	exit(1);
    }
}

void recvLoop(int sd) 
{
    int n, bytes_to_read;
    char rbuf[BUFLEN];
    char tbuf[8];
    char *bp;
    
    while(1) {
	n = 0;
	bp = rbuf;
	bytes_to_read = BUFLEN;

	while ((n = recv(sd, bp, bytes_to_read, 0)) < BUFLEN)
	{
	    bp += n;
	    bytes_to_read -= n;
	    if(*(bp-1) == '\n') {
		*bp = '\0';
		break;
	    }
	    usleep(1000);
	}

	strncpy(tbuf,rbuf,6);
	if(strcmp(tbuf,"NOTICE") != 0) {
	    printf("%s", rbuf);
	    fflush(stdout);
	}
    }
    exit(0);
}


void initChat(char *nick, char *channel, char *server, int sd)
{
    char tbuf[BUFLEN];
    
    printf("Nickname: ");
    fgets(nick, BUFLEN, stdin);

    printf("Channel: ");
    fgets(channel, BUFLEN, stdin);

    sprintf(tbuf,"NICK %s",nick);
    send(sd, tbuf, strlen(tbuf), 0);
    sprintf(tbuf,"USER fakeuser fakeuser %s :Fake Name\n", server);
    send(sd, tbuf, strlen(tbuf), 0);
    sprintf(tbuf,"JOIN %s",channel);
    send(sd, tbuf, strlen(tbuf), 0);
    channel[strlen(channel)-1] = '\0';
}
