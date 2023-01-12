#ifndef FCN_SERVER_H
#define FCN_SERVER_H

#include "chase.h"

//Global variables
int num_elements, num_prizes;   // total number of elements (all bots, players and prizes) and number of prizes
client_list *head; 
WINDOW *my_win;
int server_sock;

// Functions for lists and updates -- comments in file
client_list* create_head_client_list();
int insert_new_client(char c, int x, int y, int health, int socket_id);
int delete_client(int socket_id);
void delete_all_list();
client_list* search_position(int x, int y);
int search_letter(char c);
client_list* search_client(int socket_id);
int delete_prizes(client_list* prize);
int health_0(client_list* player);
int update_client(int socket_id, int direction);
client_list* update_bot(client_list* aux, int mod);
position_t initialize_player();
position_t initialize_bot_prizes(int bot);

// Communication -- comments in file 
void field_st2all (position_t old_pos, position_t new_pos, int flag_2msg);
void send_all_field(int flag_1msg, int sock_fd);

// Graphics
void move_client (client_list* client, int x, int y);

// Handler functions
void interupt_handler_server(int s);


#endif