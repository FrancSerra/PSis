#include"fcn_client.h"


void mng_field_status(message_fieldstat_t msg){

    switch (msg.flag){
    case 0:
        draw_player(my_win, &msg.new_pos, true);
        if (msg.new_pos.health != -1){
            draw_health(&msg.new_pos, 0);
        }
        break;

    case 1:
        if(msg.old_pos.x != msg.new_pos.x || msg.old_pos.y != msg.new_pos.y){
            draw_player(my_win, &msg.old_pos, false);
            draw_player(my_win, &msg.new_pos, true);

            if(msg.old_pos.health != msg.new_pos.health && msg.old_pos.c != BOT_CHAR){
                draw_health(&msg.new_pos, 0);
            }
        }
        break;

    case 2:
        draw_health(&msg.new_pos, 0);
        if(msg.old_pos.c != BOT_CHAR && msg.old_pos.c != msg.new_pos.c){
            draw_health(&msg.old_pos, 0);
        }
        break;

    case 3:
        draw_player(my_win, &msg.old_pos, false);
        draw_health(&msg.old_pos, 1);
        break;
    
    default:
        break;
    }

}

// Handler functions
void sig_handler(int signum){
    mvwprintw(error_win, 1,1,"Key not pressed.\n Game over!\n");
    wrefresh(error_win);
    close(client_sock);
    exit(-1);
}

void interupt_handler_client(int s){
    mvwprintw(error_win, 1,1,"Caught Ctrl C!\n\n\n");
    wrefresh(error_win);
    close(client_sock);
    exit(-1); 
}
