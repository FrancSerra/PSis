#include "chase.h"

// Global variables
int aux_health0; // 1 if health reached zero
WINDOW *my_win;
int len;

void* rcv_thread(void* arg) {

    int sock_fd = *(int*)arg;
    int nbytes;
    message_t in_msg, out_msg;
    char msg_client[ALOC_MAX];
    message_ballmov_t in_msg_ballmov;

    while(1){
        nbytes = recv(sock_fd, &in_msg, sizeof(in_msg), 0);
        if(nbytes != sizeof(message_t)){
            break;
        }

        switch (in_msg.type) {
        case health0:
            aux_health0 = 1;
            mvwprintw(error_win, 1,1,"Your health reached 0. Press 'y' to continue playing.\n");
            wrefresh(error_win);
            alarm(TIME_OUT);
            
            while(1){
                if(aux_health0 == 0){
                    alarm(0); //Cancel previous alarm

                    // Sends a continue_game message
                    out_msg = msg2send(continue_game, UNUSED_CHAR, -1, -1, -1, -1);
                    send(client_sock, &out_msg, sizeof(message_t), 0); 
                    break;
                }
            }       
            break;
        
        case field_stat:
            nbytes = recv(sock_fd, &in_msg_ballmov, sizeof(in_msg_ballmov), 0);
            if(nbytes != sizeof(message_ballmov_t) || in_msg_ballmov.type != field_stat){
                break;
            }

            //stores the field info encoded by a string
            strcpy(msg_client, in_msg_ballmov.str);
            //string decode process
            for (int j = 0; j < len; j++){
                // Deletes the previous positions
                draw_player(my_win, &field[j], false);
                if (field[j].health != -1){
                    // Deletes the previous health
                    draw_health(&field[j], 1, false);
                }
            }
            len = in_msg_ballmov.num_elem;
            field = decode_msg_field(len, msg_client, my_win);
            break;
        default:
            break;
        }
    }
    
    mvwprintw(error_win, 1,1,"Communication error\n");
    wrefresh(error_win);
    free(field);
    close(sock_fd);
    exit(-1);
}

int main(int argc, char *argv[]){

    // Client receives the server adress as command line argument
    if (argc != 3){
        printf("Error: Missing server adress or port.\n");
        exit(1);
    }

    // Create the socket 
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(client_sock == -1){
        perror("socket: ");
        exit(-1);
    }

    // Store server information in a struct sockaddr_in
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(argv[2]));

    message_t in_msg, out_msg;
    int nbytes;
    int key = -1;
    pthread_t thread_id;
    aux_health0 = 0;
    field = (position_t *)malloc(sizeof(position_t)*ALOC_MAX);
    len = 0;

    // If the provided address is invalid, exit.
    if(inet_aton(argv[1], &server_address.sin_addr) < 1){
        printf("Error: Not a valid IP address: \n");
        exit(-1);
    }

    // Register signal handler
    signal(SIGALRM,sig_handler); 

    // Connect to Server
    int err = connect(client_sock, (struct sockaddr*) &server_address, sizeof(server_address));
    if(err == -1){
        perror("connect");
        free(field);
        close(client_sock);
        exit(-1);
    }

    out_msg = msg2send(conn, UNUSED_CHAR, -1, -1, -1, -1);
    send(client_sock, &out_msg, sizeof(message_t), 0); 

    nbytes = recv(client_sock, &in_msg, sizeof(in_msg), 0);

    if (nbytes != sizeof(message_t)) {
        perror("Error");
        free(field);
        close(client_sock);
        exit(-1);
    }

    switch (in_msg.type){
        case error:
            printf("Error connecting this client.\n");
            free(field);
            close(client_sock);
            exit(-1);
        case ball_info:  // stores the assigned parameters by the server
            player.c = in_msg.c;
            player.x = in_msg.x;
            player.y = in_msg.y;
            player.health = in_msg.health;
            
            field[0] = player;
            len = 1;
            break; // go to the main loop
        default:
            printf("Error: You have been disconnected.\n");
            free(field);
            close(client_sock);
            exit(-1);
    }

    //creates the window and draws the player
    my_win = generate_window();
    draw_player(my_win, &player, true);
    draw_health(&player, 0, true);

    // Create thread
    pthread_create(&thread_id, NULL, rcv_thread, (void*)&client_sock);

    //waits for user input
    while (key != 27 && key != 'q'){  // ESC and q inputs close que program

        key = wgetch(my_win);

        if (aux_health0 == 1 && (key == 'Y' || key == 'y')){
            aux_health0 = 0;
            mvwprintw(error_win, 1,1,"                                                           ");
            wrefresh(error_win);
        }


        if ((key == KEY_LEFT || key == KEY_RIGHT || key == KEY_UP || key == KEY_DOWN) && aux_health0 == 0){
            // sends the inputed key to the server
            out_msg = msg2send(ball_mov, UNUSED_CHAR, -1, -1, key, -1);
            send(client_sock, &out_msg, sizeof(message_t), 0);
        }

    } 
    mvwprintw(error_win, 0,1,"\n");
    wrefresh(error_win);   
    free(field);
    close(client_sock);
}