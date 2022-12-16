#include "chase.h"

int main(int argc, char *argv[]){

    if (argc != 2){
        printf("Error: Missing recipient IP.\n");
        exit(1);
    }

    int client_sock;
    struct sockaddr_un server_address, client_address;
    int server_address_size = sizeof(server_address);

    client_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (client_sock == -1){
		perror("socket: ");
		exit(-1);
	}

    client_address.sun_family = AF_UNIX;
    sprintf(client_address.sun_path, "/tmp/client%d", getpid());

    unlink(client_address.sun_path);
    int err = bind(client_sock, (const struct sockaddr *) &client_address, sizeof(client_address));
    if(err == -1) {
        perror("bind");
        exit(-1);
    }

    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, argv[1]);

    int client_pid;
    client_pid =  getpid();

    message_t in_msg, out_msg;
    message_ballmov_t in_msg_ballmov;
    // position_t* field = (position_t *) malloc(sizeof(position_t)*(MAX_PLAYERS+MAX_BOTS+MAX_PRIZES)+1);
    int n_bytes; 
    // int len = 0;

    out_msg = msg2send(conn, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
    sendto(client_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr*) &server_address, sizeof(server_address));

    n_bytes = recvfrom(client_sock, &in_msg, sizeof(message_t), 0, (struct sockaddr *) &server_address, &server_address_size);
    if(n_bytes == sizeof(message_t)) {
        switch(in_msg.type) {
            case error:
                printf("Error connecting this client.\n");
                exit(-1);
        
            case ball_info:
                player.c = in_msg.c;
                player.x = in_msg.x;
                player.y = in_msg.y;
                player.health = in_msg.health;
                // field[0] = player;
                // len = 1;
                break; // go to the main loop

            default:
                out_msg = msg2send(disconn, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
                sendto(client_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr*) &server_address, sizeof(server_address));
                printf("Error: You have been disconnected.\n");
                exit(-1);
        }
    }
    else { // error n_bytes != sizeof(message_t): message not received
        out_msg = msg2send(disconn, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
        sendto(client_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr*) &server_address, sizeof(server_address));
        printf("Error: You have been disconnected.\n");
        exit(-1);
    }

    // Main Loop
    
    WINDOW* my_win = generate_window();
    draw_player(my_win, &player, true);
    draw_health(&player, 0, true);	         

    long int key = -1;
    while(key != 27 && key!= 'q'){
        key = wgetch(my_win);	
        if (key == KEY_LEFT || key == KEY_RIGHT || key == KEY_UP || key == KEY_DOWN){
            out_msg = msg2send(ball_mov, client_pid, UNUSED_CHAR, -1, -1, key, -1);
            sendto(client_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr*) &server_address, sizeof(server_address));

            n_bytes = recvfrom(client_sock, &in_msg_ballmov, sizeof(message_ballmov_t), 0, (struct sockaddr *) &server_address, &server_address_size);

            if(n_bytes == sizeof(message_ballmov_t)) {

                switch (in_msg_ballmov.type){
                case error:
                    printf("\033[11B");
                    printf("\033[6D");
                    printf("GAME OVER! Your health reached 0.\n");
                    printf("\033[1B");
                    printf("\033[49D");
                    exit(0);

                case field_stat:	
                    // for (int j=0; j<len; j++){
                    //     draw_player(my_win,&field[j],false);
                    //     if(field[j].health != -1){
                    //        draw_health(&field[j], 2, false); 
                    //     }
                    // }

                    // field = in_msg_ballmov.arr_field;
                    // len = in_msg_ballmov.num_elem;
                    // for(int i=0; i<len; i++){
                    //     draw_player(my_win, &field[i], true);
                    //     if(field[i].health != -1){
                    //        draw_health(&field[i], 1, false); 
                    //     }
                    // }
                    break;

                default:
                    break;
                }
            }
        }
         	
    }  

    printf("\033[11B");
    printf("\033[6D");
    printf("GAME OVER!\n");
    printf("\033[1B");
    printf("\033[10D");
    out_msg = msg2send(disconn, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
    sendto(client_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr*) &server_address, sizeof(server_address));
    exit(0);
}