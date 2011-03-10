#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include "../user.h"
#include "server.h"

void get_host_name(user* u);

void send_motd(server* srv, user* u);

void send_welcome_info(server* srv, user* u);

#endif
