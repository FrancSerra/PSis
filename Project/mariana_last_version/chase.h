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

#define SERVER_ADDRESS "/tmp/server_socket"
#define WINDOW_SIZE 20
#define MAX_PLAYERS 10
#define UNUSED_CHAR 35 // # in ASCII
#define INITIAL_HEALTH 10

typedef enum msg_type{
    conn, ball_info, ball_mov, field_stat, health0, disconn, error
} msg_type;

typedef struct message_t{
    msg_type type;
    int pid;
    char c;
    int x;
    int y;
    int direction;
    int health;
} message_t;

typedef struct position_t {
    int x, y;
    char c;
    int health;
} position_t;

typedef struct client_list{
    int pid;
    char c;
    int x, y;
    int health;
    struct client_list *next;
} client_list;

// Global variables
int num_players;
WINDOW * message_win; // Prof
position_t player; // Prof


// Lists
client_list* create_head_client_list();
int insert_new_client(client_list* head, int pid, char c, int x, int y, int health);
int delete_client(client_list* head, int pid, WINDOW* win);
void print_client_list(client_list *node);
client_list* search_position(client_list* head, int x, int y);
int search_letter(client_list* head, char c);
client_list* search_client(client_list* head, int pid);
client_list* update_client(client_list* head, int pid, int direction, WINDOW* win);

// Comms
position_t initialize_player(client_list* head);
message_t msg2send(msg_type type, int pid, char c, int x, int y, int direction, int health);

// Graphics
WINDOW* generate_window();
void new_player (position_t * player, char c);
void draw_player(WINDOW *win, position_t * player, int delete);
void draw_health(position_t * player, int to_do, int conn_client);
