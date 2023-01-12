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
#include <signal.h>

//The port number should be between 4000 and 40000, and needs to be converted with the htons function.
#define SOCK_PORT 5000
#define WINDOW_SIZE 20
#define MSG_BOX_HEIGHT 18
#define INITIAL_HEALTH 10
#define UNUSED_CHAR 35 // # in ASCII
#define ZERO_ASCII 48 // 0 in ASCII
#define TIME_OUT 10 // seconds waiting for continue_game
#define TIME_GENERATE_PRIZE 5 // seconds waiting for a new prize to be generated
#define TIME_UPDATE_BOTS 3 // seconds waiting for updating bots positions
#define MAX_BOTS 10
#define MIN_BOTS 1
#define MAX_PRIZES 10
#define INIT_PRIZES 5
#define MAX_VALUE_PRIZES 53 // 5 in ASCII
#define MIN_VALUE_PRIZES 49 // 1 in ASCII
#define BOT_CHAR 42 // * in ASCII

#define COLOR 1

// Messages types
typedef enum msg_type{
    conn, ball_info, ball_mov, field_stat, health0, continue_game, error
} msg_type;

// Struct messages
typedef struct message_t{
    msg_type type;      // message type
    char c;             // client characters
    int x;              // client position x
    int y;              // client position y
    int direction;      // direction to move
    int health;         // client health
} message_t;

// Struct position
typedef struct position_t {
    int x, y;           // position x,y
    char c;             // client character
    int health;         // client health
} position_t;

// Struct messages to send in field_status message type
typedef struct message_fieldstat_t{
    int flag;        
    position_t old_pos;     
    position_t new_pos;  
} message_fieldstat_t;

// Struct list of clients
typedef struct client_list{
    char c;                    // client character
    int x, y;                  // client position x,y
    int health;                // client health
    int socket_id;             // socket id
    struct client_list *next;  // pointer to the next client/element in the list
} client_list;


// Global variables 
WINDOW * message_win;          // message window
WINDOW * error_win;            // error window
int char_client; 


// Functions for communications
message_t msg2send(msg_type type, char c, int x, int y, int direction, int health);
message_fieldstat_t msg2send_fieldstat(int flag, position_t old_pos, position_t new_pos);

// Functions for graphical part (windows and draw players, bots and prizes in the field) -- comments in file chase.c
WINDOW* generate_window();
void reset_windows(WINDOW* my_win);
void draw_player(WINDOW *win, position_t * player, int delete);
void draw_health(position_t * player, int to_do);

#endif