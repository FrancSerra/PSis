#ifndef FCN_CLIENT_H
#define FCN_CLIENT_H

#include "chase.h"

// Global variables
int client_sock;
int aux_health0; // 1 if health reached zero
WINDOW *my_win;

// Functions
void mng_field_status(message_fieldstat_t msg);

// Handler functions
void sig_handler(int signum);
void interupt_handler_client(int s);

#endif