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

int main (int argc, char **argv)
{
    int n, bytes_to_read;
    int sd, port;
    struct hostent*hp;
    struct sockaddr_in server;
    char  *host, *bp, rbuf[BUFLEN], sbuf[BUFLEN], tbuf[BUFLEN];
    char str[16];
    char nick[BUFLEN];
    char channel[BUFLEN];
    bool running = true;

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

    // Create the socket
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
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
    if (connect (sd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
	fprintf(stderr, "Can't connect to server\n");
	perror("connect");
	exit(1);
    }

    if(fork() > 0) {
		
	while(running == true) {
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
	    }

	    strncpy(tbuf,rbuf,13);
	    if(strcmp(tbuf,"NOTICE AUTH :") != 0) {
		printf("%s", rbuf);
		fflush(stdout);
	    }
	}
	exit(0);
    }

    printf("Nickname: ");
    fgets(nick, BUFLEN, stdin);
    printf("Channel: ");
    fgets(channel, BUFLEN, stdin);
   
    sprintf(sbuf,"NICK %s",nick);
    send(sd, sbuf, strlen(sbuf), 0);
    sprintf(sbuf,"USER fakeuser fakeuser %s :Fake Name\n",argv[1]);
    send(sd, sbuf, strlen(sbuf), 0);
    sprintf(sbuf,"JOIN %s",channel);
    send(sd, sbuf, strlen(sbuf), 0);
    channel[strlen(channel)-1] = '\0';

    while(1) {
	fgets (tbuf, BUFLEN, stdin);
        sprintf(sbuf, "PRIVMSG %s :%s", channel, tbuf);
	send (sd, sbuf, strlen(sbuf), 0);
    }

    close (sd);
    return 0;
}
