#include "fcn_server.h"

extern pthread_mutex_t* ptr_mtx;
extern pthread_rwlock_t* ptr_rwlock_list;

// Thread for bots
void* bots_thread(void* arg) {
    srand(time(NULL));
    int num_bots;
    position_t init_pos;
    int new_bot_err, rand_num;
    client_list* aux;

    // Generates a random number of bots between 1 and 10
    // Repeats until there is left at least one position empty in the field
    do {
        num_bots = (rand() % (MAX_BOTS - MIN_BOTS + 1)) + MIN_BOTS;
    } while (num_bots + INIT_PRIZES >= (WINDOW_SIZE-2)*(WINDOW_SIZE-2));

    // Initializes the bots    
    for (int i = 0; i < num_bots; i++) {
        // Initializes bots. Assigns it's representation an empty board position. 
        init_pos = initialize_bot_prizes(true);

        // Write lock
        pthread_rwlock_wrlock(ptr_rwlock_list);
        // Adds the bots into the list of elements
        // Sock_fd doesn't have a meaning for bots (assigned value -1)
        new_bot_err = insert_new_client(init_pos.c, init_pos.x, init_pos.y, init_pos.health, -1); 
        // Write unlock
        pthread_rwlock_unlock(ptr_rwlock_list);

        if (new_bot_err != -1){ 
            // Draws in the board            
            draw_player(my_win, &init_pos, true);

            // Mutex lock
            pthread_mutex_lock(ptr_mtx);
            // Increments the number of elements
            num_elements++;
            // Mutex unlock
            pthread_mutex_unlock(ptr_mtx);
        }
        else{
            // Repeats the iteration
            i--;
        }
    }

    while(1) {
        // Waits 3 seconds
        sleep(TIME_UPDATE_BOTS);

        aux = head;

        // Updates the position of the bots
        for (int i = 0; i < num_bots; i++) {
            // Random number between 1 and 4
            rand_num = (rand() % 4) + 1; 
            aux = update_bot(aux, rand_num);
        }

    }

}

// Thread for prizes
void* prizes_thread(void* arg){
    num_prizes = 0;
    position_t init_pos;
    int new_prize_err;

    // Window game must have at least +2 positions than the number of initial prizes so at least one bot and one player can join
    if(INIT_PRIZES >= ((WINDOW_SIZE-2)*(WINDOW_SIZE-2))-1) {
        printf("Window size too small.\n");
        close(server_sock);
        exit(-1);
    }

    // Initializes the 5 prizes
    for (int i = 0; i < INIT_PRIZES; i++){
        
        // Initializes. Assigns it's representation (1, 2, etc) and an empty board position. 
        init_pos = initialize_bot_prizes(false);

        // Write lock
        pthread_rwlock_wrlock(ptr_rwlock_list);
        // Adds the prize into the list of elements
        // Sock_fd doesn't have a meaning for prizes (assigned value -1)
        new_prize_err = insert_new_client(init_pos.c, init_pos.x, init_pos.y, init_pos.health, -1); 
        
        // Write unlock
        pthread_rwlock_unlock(ptr_rwlock_list);
        if (new_prize_err != -1){ 
            // Draws in the board            
            draw_player(my_win, &init_pos, true);
            
            // Mutex lock
            pthread_mutex_lock(ptr_mtx);
            // Increments the number of prizes and elements
            num_prizes++;
            num_elements++;
            // Mutex unlock
            pthread_mutex_unlock(ptr_mtx);
        }
        else{
            // Repeats iteration
            i--;
        }

    }

    while(1) {
        // Waits 5 seconds to generate a new prize
        sleep(TIME_GENERATE_PRIZE);

        // Only generates a new prize if the maximum number definied was not reached and if there are positions available in the field
        if(num_prizes < MAX_PRIZES && num_elements < (WINDOW_SIZE-2)*(WINDOW_SIZE-2)) {
            // Initialize one prize. Assigns it's representation (1, 2, etc) and an empty board position. 
            init_pos = initialize_bot_prizes(false);

            // Write lock
            pthread_rwlock_wrlock(ptr_rwlock_list);
            // Adds the Prizes into the list of elements
            // Sock_fd doesn't have a meaning for prizes (assigned value -1)
            new_prize_err = insert_new_client(init_pos.c, init_pos.x, init_pos.y, init_pos.health, -1); 
            // Write unlock
            pthread_rwlock_unlock(ptr_rwlock_list);

            if (new_prize_err != -1){    
                // Draws in the board            
                draw_player(my_win, &init_pos, true);

                // Mutex lock
                pthread_mutex_lock(ptr_mtx);
                // Increments the number of prizes and elements
                num_prizes++;
                num_elements++;
                // Mutex unlock
                pthread_mutex_unlock(ptr_mtx);
                
                // Updates the field for every client
                field_st2all(init_pos, init_pos, 0);
            }
        }

    }

    
}

// Thread for clients
void* client_thread(void* arg){

    int sock_fd = *(int*)arg; // Socket_id passed as an argument
    int nbytes, err_update, delete_client_err;
    message_t in_msg, out_msg;
    client_list* client;  
    position_t new_play, old_pos;

    while(1){
        // Receives messages from the assigned client
        nbytes = recv(sock_fd, &in_msg, sizeof(in_msg), 0);
        if(nbytes != sizeof(message_t) || (in_msg.type != ball_mov && in_msg.type != continue_game)){
            // In case the socket of that client is closed, or it does not send a message of type continue_game or ball_movement
            break;
        }

        // Cleans error window of the server
        mvwprintw(error_win, 1,1,"\n\n\n\n");
        wrefresh(error_win);

        // In case it receives a message of tyoe continue_game
        if(in_msg.type == continue_game) {
            
            // Read lock
            pthread_rwlock_rdlock(ptr_rwlock_list);
            // Searchs that client in the list
            client = search_client(sock_fd);
            // Read unlock
            pthread_rwlock_unlock(ptr_rwlock_list);

            // Write lock
            pthread_rwlock_wrlock(ptr_rwlock_list);
            // Client's health becomes maximum again
            client->health = INITIAL_HEALTH;
            // Write unlock
            pthread_rwlock_unlock(ptr_rwlock_list);

            // Read lock
            pthread_rwlock_rdlock(ptr_rwlock_list);
            new_play.c = client->c;
            new_play.x = client->x;
            new_play.y = client->y;
            new_play.health = client->health;
            // Read unlock
            pthread_rwlock_unlock(ptr_rwlock_list);
            
            // Updathes the health of that client on the message window of the server 
            draw_health(&new_play, 0);

            // Sends all field to this client
            send_all_field(2, sock_fd);
            // Sends the update of this client's health to every other client
            field_st2all (new_play, new_play, 2);
        }

        // In case it receives a message of type ball_movement
        if (in_msg.type == ball_mov) {

            // Updates the list and that client on the server screen
            err_update = update_client(sock_fd, in_msg.direction);
        
            // If an error occurs, returns an error message to the player
            if (err_update == -1){
                out_msg = msg2send(error, UNUSED_CHAR, -1, -1, -1, -1);
                send(sock_fd, &out_msg, sizeof(message_t), 0);
            }
        }
    }

    // In case the socket of that client is closed, or it does not send a message of type continue_game or ball_movement
    // Case there is a break in the while(1) loop

    // Read lock
    pthread_rwlock_rdlock(ptr_rwlock_list);

    // Search the client by its socket_id
    client = search_client(sock_fd);
    old_pos.c = client->c;
    old_pos.x = client->x;
    old_pos.y = client->y;
    old_pos.health = client->health;

    // Read unlcok
    pthread_rwlock_unlock(ptr_rwlock_list);

    // Write lock
    pthread_rwlock_wrlock(ptr_rwlock_list);
    // Deletes the client from the list of clients and closes its socket
    delete_client_err = delete_client(sock_fd);

    // Write unlock
    pthread_rwlock_unlock(ptr_rwlock_list);

    if (delete_client_err == -1){
        // Prints the error on the server error window
        mvwprintw(error_win, 1,1,"Error disconnecting client.\n");
        wrefresh(error_win);

    }
    else{
        // Cleans server error window
        mvwprintw(error_win, 1,1,"\n\n\n\n");
        wrefresh(error_win);
        
        // Mutex lock
        pthread_mutex_lock(ptr_mtx);
        // Decreases number of players
        num_elements--;
        // Mutex unlock
        pthread_mutex_unlock(ptr_mtx);
    }

    // Updates the field to every other client
    field_st2all (old_pos, old_pos, 3);
    
    // Closes the client socket
    pthread_exit(NULL);
}

// Main thread
int main()
{
    int sock_fd;

    // Creates server socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock == -1){
		perror("socket: ");
		exit(-1);
	}

    // Store server information in a struct sockaddr_in
	struct sockaddr_in server_address, client_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(SOCK_PORT);
	server_address.sin_addr.s_addr = INADDR_ANY;
    socklen_t client_addr_size = sizeof(struct sockaddr_in);

    int on = 1;
    setsockopt (server_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (on));

    // Binds the socket
	int err = bind(server_sock, (struct sockaddr *)&server_address, sizeof(server_address));
	if(err == -1) {
		perror("bind");
		exit(-1);
	}

    // Listen for new connections
    err = listen(server_sock, 10);
    if(err == -1) {
        perror("listen");
        exit(-1);
    }

    // Declaration and initialization of local variables
    pthread_t thread_id, thread_id_prizes, thread_id_bots;
    message_t in_msg, out_msg;
    int nbytes, flag_thread;
    position_t init_pos;
    int new_client_err;
    int aux_elements;

    // Initialization of global variables
    num_elements = 0;
    head = create_head_client_list(); // Creates the linked list where all board info is stored; players, bots, prizes
    my_win = generate_window();
    char_client = 0;

    // Interrupt handler for CTRL+C
    struct sigaction sigIntHandler; 
    
    sigIntHandler.sa_handler = interupt_handler_server;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);


    // Create bots and prizes threads
    pthread_create(&thread_id_prizes, NULL, prizes_thread, NULL);
    pthread_create(&thread_id_bots, NULL, bots_thread, NULL);


    while(1){
        // Accepts new connections
        sock_fd = accept(server_sock, (struct sockaddr *) &client_address, &client_addr_size);
        // If fails accepting
        if(sock_fd < 0){
            // Prints the error
            mvwprintw(error_win, 1,1,"Error while accepting connection to server.\n");
            wrefresh(error_win);
        }
        else {
            // Cleans the error window
            mvwprintw(error_win, 1,1,"\n\n\n\n");
            wrefresh(error_win);

            // Receives ball_connect from clients
            nbytes = recv(sock_fd, &in_msg, sizeof(in_msg), 0);

            // If it receives a correct message
            if(nbytes == sizeof(message_t) && in_msg.type == conn){
                
                // Mutex lock
                pthread_mutex_lock(ptr_mtx);
                aux_elements = num_elements;
                // Mutex unlock
                pthread_mutex_unlock(ptr_mtx);

                // If there is still space available in the field for the client to join
                if (aux_elements < ((WINDOW_SIZE-2)*(WINDOW_SIZE-2))) {
                    //Initializes the player position: assignes an empty board position and unused letter
                    init_pos = initialize_player();     
                    
                    // Write lock
                    pthread_rwlock_wrlock(ptr_rwlock_list);
                    // Inserts the new player in the list
                    new_client_err = insert_new_client(init_pos.c, init_pos.x, init_pos.y, INITIAL_HEALTH, sock_fd);
                    // Write unlock
                    pthread_rwlock_unlock(ptr_rwlock_list);

                    // If successfully added to the list
                    if (new_client_err != -1){ 

                        // Mutex lock
                        pthread_mutex_lock(ptr_mtx);
                        // Increments the number of players
                        num_elements++; 
                        // Mutex unlock
                        pthread_mutex_unlock(ptr_mtx);

                        // Draws the new player and its health in the server board
                        draw_player(my_win, &init_pos, true);
                        draw_health(&init_pos, 0);

                        // Fills the message structure with the assigned position and character of the new client (ball_information type)
                        out_msg = msg2send(ball_info, init_pos.c, init_pos.x, init_pos.y, -1, INITIAL_HEALTH);

                        // Sets a flag with 1
                        flag_thread = 1;

                    }
                    // Case there is an error adding the clients
                    else{  
                        // Fills the message structure with the error type
                        out_msg = msg2send(error, UNUSED_CHAR, -1, -1, -1, -1);
                        // Sets a flag with 0
                        flag_thread = 0;
                    }
                }
                // Case there is no space available for this client
                else{ 
                    // Fills the message structure with the error type
                    out_msg = msg2send(error, UNUSED_CHAR, -1, -1, -1, -1);
                    // Sets a flag with 0
                    flag_thread = 0;
                }
                
                // Sends the message to the client (ball_info or error)
                send(sock_fd, &out_msg, sizeof(message_t), 0);
                
                // In case it was a ball_info message
                if(flag_thread == 1) {
                    // Sends a field_status message to that client with the entire field
                    send_all_field(0, sock_fd);

                    // Updates the field for every other player
                    field_st2all (init_pos, init_pos, 0);

                    // Creates a new thread for the client that just joined
                    pthread_create(&thread_id, NULL, client_thread,(void*)&sock_fd);
                }
                // In case it was an error message
                else {
                    // Closes client socket
                    close(sock_fd);
                }
            }
            else{
                // Prints error and closes client socket
                perror("Error");
                close(sock_fd);
            }
        }

    }
}
