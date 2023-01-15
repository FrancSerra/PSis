#include "fcn_client.h"

// Mutex and condition variable initialization
static pthread_mutex_t mtx_health0 = PTHREAD_MUTEX_INITIALIZER;     // mutex to access aux_health0
static pthread_cond_t condvar_health0 = PTHREAD_COND_INITIALIZER;   // condition variable to block the thread and to avoid active wait

// Thread that is always waiting for messages from the server
void* rcv_thread(void* arg) {

    int sock_fd = *(int*)arg;   // socket id
    int nbytes, flag_1msg, aux_elements;
    message_t in_msg, out_msg;
    message_fieldstat_t in_msg_fd;
    position_t updt_health;

    while(1){
        // Receives message from the server 
        nbytes = recv(sock_fd, &in_msg, sizeof(in_msg), 0);

        // If the communication fails
        if(nbytes != sizeof(message_t)){
            break;
        }

        switch (in_msg.type) {
        // Case message received is of the type health0    
            case health0:

                // Mutex lock
                pthread_mutex_lock(&mtx_health0);
                aux_health0 = 1;
                mvwprintw(error_win, 1,1,"Your health reached 0. Press 'y' to continue playing.\n");
                wrefresh(error_win);

                // Updates health on the message window 
                updt_health.c = in_msg.c;
                updt_health.health = in_msg.health;
                draw_health(&updt_health, 0);

                // Sets the timer 
                alarm(TIME_OUT);
                
                // Condition variable that blocks the thread until aux_health0 becomes 0 again
                while(aux_health0 != 0){ 
                    pthread_cond_wait(&condvar_health0, &mtx_health0);
                } 

                // Mutex unlock
                pthread_mutex_unlock(&mtx_health0);     

                // Cancel the previous alarm
                alarm(0); 

                // Sends a continue_game message
                out_msg = msg2send(continue_game, UNUSED_CHAR, -1, -1, -1, -1);
                send(client_sock, &out_msg, sizeof(message_t), 0);
                

                break;
            
            // Case message received is of the type field_status
            case field_stat:

                flag_1msg = in_msg.direction;
                aux_elements = in_msg.health;

                switch (flag_1msg){

                // In case this client just connected
                // It will receive as many messages as the number of elements
                case 0:
                    for (int i = 0; i < aux_elements; i++){

                        // Waits for num_elements messages
                        nbytes = recv(sock_fd, &in_msg_fd, sizeof(in_msg_fd), 0);
                        if(nbytes != sizeof(message_fieldstat_t)){
                            break;
                        }

                        // Draws all the game board    
                        draw_player(my_win, &in_msg_fd.new_pos, true);

                        // If it is a bot or a prize does not print the health
                        if (in_msg_fd.new_pos.health != -1){
                            draw_health(&in_msg_fd.new_pos, 0);
                        }
                    }
                    
                    break;

                // In case it is just an update of a player or bot 
                // It will receive a second message
                case 1:
                    nbytes = recv(sock_fd, &in_msg_fd, sizeof(in_msg_fd), 0);
                    if(nbytes != sizeof(message_fieldstat_t)){
                        break;
                    }

                    // Manage the field according to the flag
                    mng_field_status(in_msg_fd);
                    break;

                // In case this client has just sent a continue_game message
                // It will receive as many messages as the number of elements
                case 2:

                    // Cleans the entire window first
                    reset_windows(my_win);

                    for (int i = 0; i < aux_elements; i++){

                        // Waits for num_elements messages
                        nbytes = recv(sock_fd, &in_msg_fd, sizeof(in_msg_fd), 0);
                        if(nbytes != sizeof(message_fieldstat_t)){
                            break;
                        }

                        // Draws all the game board    
                        draw_player(my_win, &in_msg_fd.new_pos, true);

                        // If it is a bot or a prize does not print the health
                        if (in_msg_fd.new_pos.health != -1){
                            draw_health(&in_msg_fd.new_pos, 0);
                        }

                    }
                    break;
                
                default:
                    break;
                }
                break;

            default:
                break;
        }
    }
    
    // Print the error, close the socket and terminates the program
    mvwprintw(error_win, 1,1,"Communication error\n");
    wrefresh(error_win);
    close(sock_fd);
    exit(-1);
}

// Main thread
int main(int argc, char *argv[]){

    // Client receives the server adress as command line argument
    if (argc != 3){
        printf("Error: Missing server adress or port.\n");
        exit(1);
    }


    // Creates the socket 
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(client_sock == -1){
        perror("socket: ");
        exit(-1);
    }

    // Store server information in a struct sockaddr_in
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(atoi(argv[2]));

    // Declaration of variables
    message_t in_msg, out_msg;
    int nbytes;
    int key = -1;
    position_t player;            
    pthread_t thread_id;
    aux_health0 = 0;

    struct sigaction sigIntHandler; 
    
    // Interrupt handler for CTRL+C
    sigIntHandler.sa_handler = interupt_handler_client;    
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

    // If the provided address is invalid, exit.
    if(inet_aton(argv[1], &server_address.sin_addr) < 1){
        printf("Error: Not a valid IP address: \n");
        close(client_sock);
        exit(-1);
    }

    // Register signal handler
    signal(SIGALRM,sig_handler); 

    // Connect to Server
    int err = connect(client_sock, (struct sockaddr*) &server_address, sizeof(server_address));
    if(err == -1){
        perror("connect");
        close(client_sock);
        exit(-1);
    }

    // Sends as connection message to the server
    out_msg = msg2send(conn, UNUSED_CHAR, -1, -1, -1, -1);
    send(client_sock, &out_msg, sizeof(message_t), 0); 

    // Receives a message of the server
    nbytes = recv(client_sock, &in_msg, sizeof(in_msg), 0);

    if (nbytes != sizeof(message_t)) {
        perror("Error");
        close(client_sock);
        exit(-1);
    }

    switch (in_msg.type){
        // Case it receives a message of type error
        case error:
            printf("Error connecting this client.\n");
            close(client_sock);
            exit(-1);

        // Case it receives a message of type ball_information
        // Stores the assigned parameters by the server
        case ball_info:  
            player.c = in_msg.c;
            char_client = in_msg.c;
            player.x = in_msg.x;
            player.y = in_msg.y;
            player.health = in_msg.health;
            
            // Goes to the main loop
            break; 

        default:
            printf("Error: You have been disconnected.\n");
            close(client_sock);
            exit(-1);
    }

    // Creates the window and draws the player
    my_win = generate_window();
    draw_player(my_win, &player, true);
    draw_health(&player, 0);

    // Creates the thread that is always waiting for messages from the server
    pthread_create(&thread_id, NULL, rcv_thread, (void*)&client_sock);

    // waits for user input
    while (key != 27 && key != 'q'){  

        // Reads the input from the keyboard
        key = wgetch(my_win);

        // Mutex lock
        pthread_mutex_lock(&mtx_health0);

        // If the client health reached 0 and it wants to continue playing
        if (aux_health0 == 1 && (key == 'Y' || key == 'y')){
            aux_health0 = 0;

            // Cleans the error window
            mvwprintw(error_win, 1,1,"\n\n\n\n");
            wrefresh(error_win);

            // Awakes the thread that was bloked
            pthread_cond_signal(&condvar_health0);
        }

        // Mutex unlock
        pthread_mutex_unlock(&mtx_health0);

        // If the player pressed a "move" key
        if ((key == KEY_LEFT || key == KEY_RIGHT || key == KEY_UP || key == KEY_DOWN) && aux_health0 == 0){
            
            // Sends the inputed key to the server
            out_msg = msg2send(ball_mov, UNUSED_CHAR, -1, -1, key, -1);
            send(client_sock, &out_msg, sizeof(message_t), 0);
        }

    }

    // Case the player pressed 'q' or 'esc' closes the socket and terminates the program
    mvwprintw(error_win, 0,1,"\n\n\n\n");
    wrefresh(error_win);   
    close(client_sock);
    exit(-1);
}