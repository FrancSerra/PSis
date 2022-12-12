#define WINDOW_SIZE 20


typedef enum direction_t {UP, DOWN, LEFT, RIGHT} direction_t;

typedef struct remote_message_t
{   
    int msg_type; /* 0 connect   1 - move    2 - Error */
    char ch;
    int health;
    direction_t direction ;

}remote_message_t;

typedef struct players_info_t
{
    char ch;
    int pos_x, pos_y;
} players_info_t;

#define SERVER_ADDRESS "/tmp/server_socket"
#define WINDOW_SIZE 20
