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