#include "chase.h"

int main(int argc, char *argv[]){

    // Client receives the server adress as command line argument
    if (argc != 3){
        printf("Error: Missing server adress or port.\n");
        exit(1);
    }

    // Create the socket 
    int client_sock;
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
     long int key = -1;

    // If the provided address is invalid, exit.
    if(inet_aton(argv[1], &server_address.sin_addr) < 1){
        printf("Error: Not a valid IP address: \n");
        exit(-1);
    }

    // generates own ID
    int client_pid = getpid();

    // Connect to Server
    int err = connect(client_sock, (struct sockaddr*) &server_address, sizeof(server_address));
    if(err == -1){
        perror("connect");
        close(client_sock);
        exit(-1);
    }

    out_msg = msg2send(conn, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
    send(client_sock, &out_msg, sizeof(message_t), 0); 

    nbytes = recv(client_sock, &in_msg, sizeof(in_msg), 0);

    if (nbytes != sizeof(message_t)) {
        perror("Error");
        close(client_sock);
        exit(-1);
    }

    switch (in_msg.type){
        case error:
            printf("Error connecting this client.\n");
            close(client_sock);
            exit(-1);
        case ball_info:  // stores the assigned parameters by the server
            player.c = in_msg.c;
            player.x = in_msg.x;
            player.y = in_msg.y;
            player.health = in_msg.health;
            
            // field[0] = player;
            // len = 1;
            break; // go to the main loop
        default:
            printf("Error: You have been disconnected.\n");
            close(client_sock);
            exit(-1);
    }

    //creates the window and draws the player
    WINDOW *my_win = generate_window();
    draw_player(my_win, &player, true);
    draw_health(&player, 0, true);

    //waits for user input
    while (key != 27 && key != 'q'){  // ESC and q inputs close que program
        key = wgetch(my_win);

        if (key == KEY_LEFT || key == KEY_RIGHT || key == KEY_UP || key == KEY_DOWN){

            // sends the inputed key to the server
            out_msg = msg2send(ball_mov, client_pid, UNUSED_CHAR, -1, -1, key, -1);
            send(client_sock, &out_msg, sizeof(message_t), 0);

            // // receives the answer
            // n_bytes = recvfrom(client_sock, &in_msg_ballmov, sizeof(message_ballmov_t), 0, (struct sockaddr *)&server_address, (socklen_t *)&server_address_size);

            // if (n_bytes == sizeof(message_ballmov_t)){

            //     switch (in_msg_ballmov.type){

            //     case error:
            //         printf("\033[41B");
            //         printf("\033[6D");
            //         printf("GAME OVER! Your health reached 0.\n");
            //         printf("\033[1B");
            //         printf("\033[49D");
            //         exit(0);

            //     case field_stat:
            //         //stores the field info encoded by a string
            //         strcpy(msg_client, in_msg_ballmov.str);

            //         //string decode process
            //         for (int j = 0; j < len; j++){
            //             // Deletes the previous positions
            //             draw_player(my_win, &field[j], false);

            //             if (field[j].health != -1){
            //                 // Deletes the previous health
            //                 draw_health(&field[j], 2, false);
            //             }
            //         }

            //         len = in_msg_ballmov.num_elem;
            //         field = decode_msg_field(len, msg_client, my_win);
            //         break;

            //     default:
            //         break;
            //     }
            // }
        }
    }





    
    // while(1) {
    //     out_msg = msg2send(ball_mov, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
    //     send(client_sock, &out_msg, sizeof(message_t), 0); 
            
    //     sleep(1);
    // }
    
    
    close(client_sock);
}