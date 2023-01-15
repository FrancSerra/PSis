// File chase.h common to both the server and the client

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

#define SOCK_PORT 5000          // socket port
#define WINDOW_SIZE 20          // window size
#define MSG_BOX_HEIGHT 18       // height of message window
#define INITIAL_HEALTH 10       // initial health of players
#define UNUSED_CHAR 35          // # in ASCII
#define ZERO_ASCII 48           // 0 in ASCII
#define BOT_CHAR 42             // * in ASCII
#define TIME_OUT 10             // seconds waiting for continue_game
#define TIME_GENERATE_PRIZE 5   // seconds waiting for a new prize to be generated
#define TIME_UPDATE_BOTS 3      // seconds waiting for updating bots positions
#define MAX_BOTS 10             // maximum number of bots
#define MIN_BOTS 1              // minimum number of bots
#define MAX_PRIZES 10           // maximum number of prizes
#define INIT_PRIZES 5           // initial number of prizes
#define MAX_VALUE_PRIZES 53     // maximum value of a prize (5 in ASCII) 
#define MIN_VALUE_PRIZES 49     // minimum value of a prize (1 in ASCII) 
#define COLOR 1                 // 1 for use of colours 0 not to use

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
    char c;             // character
    int health;         // health
} position_t;

// Struct messages to send in field_status message type
typedef struct message_fieldstat_t{
    int flag;            // flag of the second message
    position_t old_pos;  // old position       
    position_t new_pos;  // new position
} message_fieldstat_t;

// Struct list of clients, bots and prizes
typedef struct client_list{
    char c;                    // character
    int x, y;                  // position x,y
    int health;                // health
    int socket_id;             // socket id
    struct client_list *next;  // pointer to the next element in the list
} client_list;


// Global variables 
WINDOW * message_win;          // message window
WINDOW * error_win;            // error window
int char_client;               // player character


// Functions for communications
message_t msg2send(msg_type type, char c, int x, int y, int direction, int health);
message_fieldstat_t msg2send_fieldstat(int flag, position_t old_pos, position_t new_pos);

// Functions for graphical part
WINDOW* generate_window();
void reset_windows(WINDOW* my_win);
void draw_player(WINDOW *win, position_t * player, int delete);
void draw_health(position_t * player, int to_do);

#endif