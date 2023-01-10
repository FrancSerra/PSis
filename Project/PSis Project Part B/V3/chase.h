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
#include <pthread.h>
#include <stddef.h>
#include <arpa/inet.h>
#include <netinet/in.h>

//The port number should be between 4000 and 40000, and needs to be converted with the htons function.
#define SOCK_PORT 5000
#define WINDOW_SIZE 20
#define INITIAL_HEALTH 10
#define UNUSED_CHAR 35 // # in ASCII
#define ZERO_ASCII 48 // 0 in ASCII
#define TIME_OUT 10



#define MAX_PLAYERS 10 // !!!!!!!!!!!

typedef enum msg_type{
    conn, ball_info, ball_mov, field_stat, health0, continue_game, error
} msg_type;

// Struct messages
typedef struct message_t{
    msg_type type;      // message type
    int pid;            // client PID
    char c;             // client characters
    int x;              // client position x
    int y;              // client position y
    long int direction; // direction to move
    int health;         // client health
} message_t;

// Struct position
typedef struct position_t {
    int x, y;           // position x,y
    char c;             // client character
    int health;         // client health
} position_t;

// Struct list of clients
typedef struct client_list{
    int pid;                   // client PID
    char c;                    // client character
    int x, y;                  // client position x,y
    int health;                // client health
    int socket_id;             // socket id
    struct client_list *next;  // pointer to the next client/element in the list
} client_list;

// Global variables 
int num_elements, num_prizes;   // total number of players, bots and prizes
WINDOW * message_win;          // message window
position_t player;             // information of player 
int client_sock;

// Functions for lists and updates -- comments in file chase.c
client_list* create_head_client_list();
int insert_new_client(client_list* head, int pid, char c, int x, int y, int health, int socket_id);
int delete_client(client_list* head, int pid, WINDOW* win);
client_list* search_position(client_list* head, int x, int y);
int search_letter(client_list* head, char c);
client_list* search_client(client_list* head, int pid);
int delete_prizes(client_list* head, client_list* prize, WINDOW* win);
// int health_0(client_list* head, client_list* player, WINDOW* win);
int update_client(client_list* head, int pid, int direction, WINDOW* win);

// Functions for communications (initialize and messages) -- comments in file chase.c
message_t msg2send(msg_type type, int pid, char c, int x, int y, long int direction, int health);
position_t initialize_player(client_list* head);

// Functions for graphical part (windows and draw players, bots and prizes in the field) -- comments in file chase.c
WINDOW* generate_window();
void draw_player(WINDOW *win, position_t * player, int delete);
void draw_health(position_t * player, int to_do, int conn_client);
void move_client (client_list* client, WINDOW* win, int x, int y);

// Handler functions
void sig_handler(int signum);

#endif