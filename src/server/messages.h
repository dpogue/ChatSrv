#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#include <cstdio>
#include "../user.h"

void send_message(user* client, char* msg);

char* joinmsg(user* sender, char* receiver);
char* privmsg(user* sender, char* receiver, char* msg);
char* partmsg(user* sender);

#endif
