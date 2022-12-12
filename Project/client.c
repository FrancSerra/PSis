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

#include "library.h"
#define SERVER_ADDRESS "/tmp/server_socket"


WINDOW * message_win;


typedef struct player_position_t{
    int x, y;
    char c;
} player_position_t;

void new_player (player_position_t * player, char c){
    player->x = WINDOW_SIZE/2;
    player->y = WINDOW_SIZE/2;
    player->c = c;
}

void draw_player(WINDOW *win, player_position_t * player, int delete){
    int ch;
    if(delete){
        ch = player->c;
    }else{
        ch = ' ';
    }
    int p_x = player->x;
    int p_y = player->y;
    wmove(win, p_y, p_x);
    waddch(win,ch);
    wrefresh(win);
}

void move_player (player_position_t * player, int direction){
    if (direction == KEY_UP){
        if (player->y  != 1){
            player->y --;
        }
    }
    if (direction == KEY_DOWN){
        if (player->y  != WINDOW_SIZE-2){
            player->y ++;
        }
    }
    

    if (direction == KEY_LEFT){
        if (player->x  != 1){
            player->x --;
        }
    }
    if (direction == KEY_RIGHT)
        if (player->x  != WINDOW_SIZE-2){
            player->x ++;
    }
}

player_position_t p1;

int main(int argc, char *argv[]){
    //Se não der input de dois argumentos sai logo
    if (argc != 2){
        printf("Error:  Missing recipient IP.\nYou must provide also the server IP as a second argument");
        exit(1);
    }
    
    int client_sock;
    struct sockaddr_un server_address, client_address;
    remote_message_t in_msg, out_msg;


    //SOCKET CREATION
    client_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (client_sock == -1){
		perror("socket: ");
		exit(-1);
	}

    //////////////////// CLIENT SOCKET IDENTIFICATION ////////////////////
    client_address.sun_family = AF_UNIX;
    sprintf(client_address.sun_path, "/tmp/client%d", getpid());

    unlink(client_address.sun_path);
    int err = bind(client_sock, (const struct sockaddr *) &client_address, sizeof(client_address));
    if(err == -1) {
        perror("bind");
        exit(-1);
    }
    //////////////////// SERVER SOCKET IDENTIFICATION ////////////////////
    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, argv[1]);
    /////////////////////////////////////////////////////////////////////

    //Conect to the server
    out_msg.msg_type = 0;
    sendto(client_sock, &out_msg, sizeof(remote_message_t), 0, (struct sockaddr*) &server_address, sizeof(server_address));
    printf("sent %d\n", out_msg.msg_type);

    int n_bytes;
    n_bytes = recvfrom(client_sock, &in_msg, sizeof(remote_message_t), 0, (struct sockaddr *) &server_address, sizeof(server_address));
    printf("Your assigned letter is: %s.\n", &in_msg.ch);
    
    // while(1){
        
    // }
    exit(0);
}