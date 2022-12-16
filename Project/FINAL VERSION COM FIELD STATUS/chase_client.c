#include "chase.h"

int main(int argc, char *argv[]){

    // Client receives the socket Name as command line argument

    if (argc != 2){
        printf("Error: Missing socket name.\n");
        exit(1);
    }


    //////////////  Create the socket //////////////
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

    //bind to allow receiving messages
    unlink(client_address.sun_path);
    int err = bind(client_sock, (const struct sockaddr *)&client_address, sizeof(client_address));
    
    if (err == -1){
        perror("bind");
        exit(-1);
    }

    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, argv[1]);
    /////////////////////////////////////////////


    // VARIABLES declaration and initialization
    char msg_client[BUFFER_SIZE];
    message_t in_msg, out_msg;
    message_ballmov_t in_msg_ballmov;
    int n_bytes, len = 0;
    int client_pid;


    // Memory allocation to recive the field update
    position_t *field = (position_t *)malloc(sizeof(position_t) * (MAX_PLAYERS + MAX_BOTS + MAX_PRIZES) + 1);

    // generates own ID
    client_pid = getpid();



    //////////////  MAIN CLIENT CODE //////////////


    // Sends the initial conection message 
    out_msg = msg2send(conn, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
    sendto(client_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr *)&server_address, sizeof(server_address));

    // Receives answer from the server
    n_bytes = recvfrom(client_sock, &in_msg, sizeof(message_t), 0, (struct sockaddr *)&server_address, (socklen_t *)&server_address_size);
    
    if (n_bytes == sizeof(message_t)){

        switch (in_msg.type){

        case error:
            printf("Error connecting this client.\n");
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
            out_msg = msg2send(disconn, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
            sendto(client_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr *)&server_address, sizeof(server_address));
            printf("Error: You have been disconnected.\n");
            exit(-1);
        }

    }else{ // error n_bytes != sizeof(message_t): message not received
        out_msg = msg2send(disconn, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
        sendto(client_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr *)&server_address, sizeof(server_address));
        printf("Error: You have been disconnected.\n");
        exit(-1);
    }

    //////////////  MAIN Player Client LOOP //////////////


    //creates the window and draws the player
    WINDOW *my_win = generate_window();
    draw_player(my_win, &player, true);
    draw_health(&player, 0, true);

    long int key = -1;

    //waits for user input
    while (key != 27 && key != 'q'){  // ESC and q inputs close que program
        key = wgetch(my_win);

        if (key == KEY_LEFT || key == KEY_RIGHT || key == KEY_UP || key == KEY_DOWN){

            // sends the inputed key to the server
            out_msg = msg2send(ball_mov, client_pid, UNUSED_CHAR, -1, -1, key, -1);
            sendto(client_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr *)&server_address, sizeof(server_address));

            // receives the answear
            n_bytes = recvfrom(client_sock, &in_msg_ballmov, sizeof(message_ballmov_t), 0, (struct sockaddr *)&server_address, (socklen_t *)&server_address_size);

            if (n_bytes == sizeof(message_ballmov_t)){

                switch (in_msg_ballmov.type){

                case error:
                    printf("\033[41B");
                    printf("\033[6D");
                    printf("GAME OVER! Your health reached 0.\n");
                    printf("\033[1B");
                    printf("\033[49D");
                    exit(0);

                case field_stat:
                    //stores the field info encoded by a string
                    strcpy(msg_client, in_msg_ballmov.str);

                    //string decode process
                    for (int j = 0; j < len; j++){
                        draw_player(my_win, &field[j], false);

                        if (field[j].health != -1){
                            draw_health(&field[j], 2, false);
                        }
                    }

                    len = in_msg_ballmov.num_elem;
                    field = decode_msg_field(len, msg_client, my_win);
                    break;

                default:
                    break;
                }
            }
        }
    }

    // frees the memory allocated
    free(field);

    printf("\033[41B");
    printf("\033[6D");
    printf("GAME OVER!\n");
    printf("\033[1B");
    printf("\033[10D");
    out_msg = msg2send(disconn, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
    sendto(client_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr *)&server_address, sizeof(server_address));
    exit(0);
}