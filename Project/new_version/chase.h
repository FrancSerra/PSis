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

#define SERVER_ADDRESS "/tmp/server_socket"
#define WINDOW_SIZE 20
#define MAX_PLAYERS 10
#define UNUSED_CHAR 35 // # in ASCII

// Global variables
int num_players;

typedef enum msg_type{
    conn, ball_info, ball_mov, field_stat, health0, disconn, error
} msg_type;

/* typedef struct message_t{
    msg_type type;
    ball_position_t ball_pos;
} message_t; */

typedef struct message_t{
    msg_type type;
    int pid;
    char c;
    int x;
    int y;
    int direction;
    int health;
} message_t;

char ascii2char();
int check_message(message_t msg);
message_t msg2send(msg_type type, int pid, char c, int x, int y, int direction, int health);
