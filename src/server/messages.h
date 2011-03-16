#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#include <cstdio>
#include <cstring>
#include <unistd.h>
#include "../user.h"
#include "../channel.h"
#include "server.h"

void send_message(user* client, char* msg);

char* numericmsg(server* srv, user* to, int id, char* msg);
char* pongmsg(server* srv);
char* joinmsg(user* sender, channel* chan);
char* noticemsg(user* sender, char* receiver, char* msg);
char* noticemsg_s(char* sender, char* msg);
char* privmsg(user* sender, char* receiver, char* msg);
char* nickmsg(user* sender, char* name);
char* partmsg(user* sender);
char* quitmsg(user* sender, char* text);

#endif
