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
#include <unistd.h> // for funntion sleep()


#define SERVER_ADDRESS "/tmp/server_socket"
#define WINDOW_SIZE 20
#define MAX_PLAYERS 10
#define UNUSED_CHAR 35 // # in ASCII
#define INITIAL_HEALTH 10
#define MAX_HEALTH 10
#define BUFFER_SIZE 1024

typedef enum msg_type
{
    conn,
    ball_info,
    ball_mov,
    field_stat,
    health0,
    disconn,
    error,
    bot_con,
    bot_dirs
} msg_type;

typedef enum direction_t
{
    UP,
    DOWN,
    LEFT,
    RIGHT
} direction_t;



typedef struct message_request_t
{
    msg_type type;
    int pid;
    int n_bots;
    direction_t direction;

} message_request;

typedef struct message_ball_info_t
{
    int pid;
    char c;
    int x;
    int y;
} message_ball_info;

typedef struct position_t
{
    char c;
    int x;
    int y;
    int health;
} position_t;

typedef struct message_update_t
{
    msg_type type;
    position_t array_update[];
} message_update;

typedef struct client_list
{
    int pid;
    char c;
    int x, y;
    int health;
    struct client_list *next;
} client_list;

// Global variables
int num_players;

client_list *create_head_client_list();
int insert_new_client(client_list *head, int pid, char c, int x, int y, int health);
int delete_client(client_list *head, int pid);
void print_client_list(client_list *node);
int search_position(client_list *head, int x, int y);
client_list *search_player(client_list *head, int pid);
position_t initialize_player();
int check_message(message_request msg);
message_request msg2send(msg_type type, int pid, int n_bots, int direction);
void new_player(position_t *player, char c);
void draw_player(WINDOW *win, position_t *player, int delete);
void moove_player(position_t *player, int direction);
int get_pid(char *str);

void concat_list_to_struct_array(client_list *head, struct position_t *array_update[]);
char *constructor(enum msg_type type, client_list *head);
