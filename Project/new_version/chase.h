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
#define UNUSED_CHAR 35 // # in ASCII
#define INITIAL_HEALTH 10

typedef enum msg_type{
    conn, ball_info, ball_mov, field_stat, health0, disconn, error
} msg_type;

typedef enum direction_t {UP, DOWN, LEFT, RIGHT} direction_t;

typedef struct message_t{
    msg_type type; 
    int pid;
    char c;
    int x;
    int y;
    direction_t direction;
    int health;
} message_t;

typedef struct position_t {
    int x, y;
    char c;
} position_t;

typedef struct client_list{
    int pid;
    char c;
    int x, y;
    int health;
    struct client_list* next;
} client_list;

// Global variables
int num_players;
client_list* head;


client_list* create_head_client_list();
int insert_new_client(client_list* head, int pid, char c, int x, int y, int health);
int delete_client(client_list* head, int pid);
void print_client_list(client_list *node);
int search_position(client_list* head, int x, int y);
client_list* search_player(client_list* head, int pid);
position_t initialize_player();
int check_message(message_t msg);
message_t msg2send(msg_type type, int pid, char c, int x, int y, int direction, int health);
void new_player (position_t * player, char c);
void draw_player(WINDOW *win, position_t * player, int delete);
void moove_player (position_t * player, int direction);
int get_pid(char* str);
