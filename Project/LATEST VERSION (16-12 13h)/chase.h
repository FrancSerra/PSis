#ifndef CHASE_H
#define CHASE_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>  
#include <unistd.h>
#include <ncurses.h>
#include <time.h>
#include <ctype.h>

#define SERVER_ADDRESS "/tmp/server_socket"
#define WINDOW_SIZE 20
#define MAX_PLAYERS 10
#define MAX_BOTS 10
#define MAX_PRIZES 10
#define INIT_PRIZES 5
#define PRIZES_LOOP 1
#define UNUSED_CHAR 35 // # in ASCII
#define BOT_CHAR 42 // * in ASCII
#define INITIAL_HEALTH 10
#define ZERO_ASCII 48
#define MAX_VALUE_PRIZES 53 // 5 in ASCII
#define MIN_VALUE_PRIZES 49 // 1 in ASCII

typedef enum msg_type{
    conn, bot_conn, prizes_conn, ball_info, ball_mov, bot_mov, field_stat, health0, disconn, error
} msg_type;

typedef struct message_t{
    msg_type type;
    int pid;
    char c;
    int x;
    int y;
    long int direction;
    int health;
} message_t;

typedef struct position_t {
    int x, y;
    char c;
    int health;
} position_t;

typedef struct message_ballmov_t{
    msg_type type;
    int pid;
    int num_elem;
    position_t* arr_field; //array of structures position_t
} message_ballmov_t;

typedef struct client_list{
    int pid;
    char c;
    int x, y;
    int health;
    struct client_list *next;
} client_list;

// Global variables 
int num_players, num_bots, num_prizes;
WINDOW * message_win;
position_t player; 

// Lists
client_list* create_head_client_list();
int insert_new_client(client_list* head, int pid, char c, int x, int y, int health);
int delete_client(client_list* head, int pid, WINDOW* win);
void print_client_list(client_list *node);
client_list* search_position(client_list* head, int x, int y);
int search_letter(client_list* head, char c);
client_list* search_client(client_list* head, int pid);
int delete_prizes(client_list* head, client_list* prize, WINDOW* win);
int health_0(client_list* head, client_list* player, WINDOW* win);
int update_client(client_list* head, int pid, int direction, WINDOW* win);
client_list* update_bot(client_list* head, client_list* temp, int mod, WINDOW* win);
position_t* update_field(client_list* head);

// Comms
position_t initialize_player(client_list* head);
position_t initialize_bot_prizes(client_list *head, int bot);
message_t msg2send(msg_type type, int pid, char c, int x, int y, long int direction, int health);
message_ballmov_t msg2send_ballmov(msg_type type, int pid, position_t* arr_field);

// Graphics
WINDOW* generate_window();
void new_player (position_t * player, char c);
void draw_player(WINDOW *win, position_t * player, int delete);
void draw_health(position_t * player, int to_do, int conn_client);
void move_client (client_list* client, WINDOW* win, int x, int y);


#endif