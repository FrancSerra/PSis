#include "chase.h"

int check_message(message_t msg){
    switch(msg.type){
        case conn:
            break;
        case ball_info:
            break;
        case ball_mov:
            if (msg.x < 1 || msg.x > WINDOW_SIZE-2 || msg.y < 1 || msg.y > WINDOW_SIZE-2)
                return -1;
            break;
        case field_stat:
            break;
        case health0:
            break;
        case disconn:
            break;
        default:
            return -1;
    }
    return 0;
}

char ascii2char() {
    int a = 65;
    char c = a+num_players-1;

    return c;
}

message_t msg2send(msg_type type, int pid, char c, int x, int y, int direction, int health) {
    message_t out_msg;

    out_msg.type = type;
    out_msg.pid = pid;
    out_msg.c = c;
    out_msg.x = x;
    out_msg.y = y;
    out_msg.direction = direction;
    out_msg.health = health;

    return out_msg;
}