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
#define INITIAL_HEALTH 10
#define INIT_PRIZES 5
#define PRIZES_LOOP 1
#define MAX_VALUE_PRIZES 53 // 5 in ASCII
#define MIN_VALUE_PRIZES 49 // 1 in ASCII

#define UNUSED_CHAR 35 // # in ASCII
#define BOT_CHAR 42 // * in ASCII
#define DELIM 36 // $ in ASCII
#define ZERO_ASCII 48 // 0 in ASCII
#define BUFFER_SIZE 1000

// include this more
#include <netinet/in.h>
#include <arpa/inet.h>
#define SOCK_PORT 5000
//The port number should be between 4000 and 40000, and needs to be converted with the htons function.

// Messages types
typedef enum msg_type{
    conn, bot_conn, prizes_conn, ball_info, ball_mov, bot_mov, field_stat, health0, disconn, error
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

// Struct messages to send in fiel_status message type
typedef struct message_ballmov_t{
    msg_type type;          // message type
    int num_elem;           // total number of elements in the field (players, bots and prizes)
    char str[BUFFER_SIZE];  // string to send fiel information
} message_ballmov_t;

// Struct list of clients
typedef struct client_list{
    int pid;                   // client PID
    char c;                    // client character
    int x, y;                  // client position x,y
    int health;                // client health
    struct client_list *next;  // pointer to the next client/element in the list
} client_list;


// Global variables 
int num_players, num_bots, num_prizes; // total number of players, bots and prizes
WINDOW * message_win;                  // message window
position_t player;                     // information of player 


// Functions for lists and updates -- comments in file chase.c
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

// Functions to send message of type field_status -- comments in file chase.c
char* field2msg(client_list* head);
char *numToASCII(int num);
position_t* decode_msg_field(int len, char str[BUFFER_SIZE],WINDOW* win);

// Functions for communications (initialize and messages) -- comments in file chase.c
message_t msg2send(msg_type type, int pid, char c, int x, int y, long int direction, int health);
message_ballmov_t msg2send_ballmov(msg_type type, int num_elem, char str[BUFFER_SIZE]);
position_t initialize_player(client_list* head);
position_t initialize_bot_prizes(client_list *head, int bot);

// Functions for graphical part (windows and draw players, bots and prizes in the field) -- comments in file chase.c
WINDOW* generate_window();
void draw_player(WINDOW *win, position_t * player, int delete);
void draw_health(position_t * player, int to_do, int conn_client);
void move_client (client_list* client, WINDOW* win, int x, int y);


#endif