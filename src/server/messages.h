#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#include <cstdio>
#include <cstring>
#include <unistd.h>
#include "../user.h"
#include "server.h"

void send_message(user* client, char* msg);

char* numericmsg(server* srv, user* to, int id, char* msg);

char* joinmsg(user* sender, char* receiver);
char* noticemsg(user* sender, char* msg);
char* privmsg(user* sender, char* receiver, char* msg);
char* partmsg(user* sender);

#endif
