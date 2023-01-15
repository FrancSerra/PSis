#ifndef FCN_CLIENT_H
#define FCN_CLIENT_H

#include "chase.h"

// Global variables
int client_sock;    // client socket
int aux_health0;    // auxiliary variable to know if health reached zero
WINDOW *my_win;     // game window

// Function that manages the field after receiving a field_status message
void mng_field_status(message_fieldstat_t msg);

// Handler functions
void sig_handler(int signum);
void interupt_handler_client(int s);

#endif