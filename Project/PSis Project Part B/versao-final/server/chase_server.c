#include "fcn_server.h"

extern pthread_mutex_t* ptr_mtx;
extern pthread_rwlock_t* ptr_rwlock_list;


void* bots_thread(void* arg) {
    srand(time(NULL));
    int num_bots;
    position_t init_pos;
    int new_bot_err, rand_num;
    client_list* aux;

    do {
        num_bots = (rand() % (MAX_BOTS - MIN_BOTS + 1)) + MIN_BOTS;
    } while (num_bots + INIT_PRIZES >= (WINDOW_SIZE-2)*(WINDOW_SIZE-2));

    
    for (int i = 0; i < num_bots; i++) {
        // Initializes bots Assigns it's representation (1, 2, etc) and an empty board position. 
        init_pos = initialize_bot_prizes(true);

        pthread_rwlock_wrlock(ptr_rwlock_list);
        // Adds the bots into the list of clients
        new_bot_err = insert_new_client(init_pos.c, init_pos.x, init_pos.y, init_pos.health, -1); // sock_fd doesn't have a meaning for the prizes
        pthread_rwlock_unlock(ptr_rwlock_list);

        if (new_bot_err != -1){ 
            // draws in the board            
            draw_player(my_win, &init_pos, true);

            // increments the number of elements
            pthread_mutex_lock(ptr_mtx);
            num_elements++;
            pthread_mutex_unlock(ptr_mtx);
        }
        else{
            i--;
            //close(server_sock);
            //exit(-1);
        }
    }

    while(1) {
        sleep(TIME_UPDATE_BOTS);

        aux = head;
        for (int i = 0; i < num_bots; i++) {
            rand_num = (rand() % 4) + 1; // random number between 1 and 4
            aux = update_bot(aux, rand_num);
        }

    }

}

void* prizes_thread(void* arg){
    num_prizes = 0;
    position_t init_pos;
    int new_prize_err;

    if(INIT_PRIZES >= (WINDOW_SIZE-2)*(WINDOW_SIZE-2)) {
        printf("Window size too small.");
        close(server_sock);
        exit(-1);
    }

    for (int i = 0; i < INIT_PRIZES; i++){
        
        // Initializes prize(s) Assigns it's representation (1, 2, etc) and an empty board position. 
        init_pos = initialize_bot_prizes(false);
        pthread_rwlock_wrlock(ptr_rwlock_list);
        // Adds the Prizes(s) into the list of clients
        new_prize_err = insert_new_client(init_pos.c, init_pos.x, init_pos.y, init_pos.health, -1); // sock_fd doesn't have a meaning for the prizes
        pthread_rwlock_unlock(ptr_rwlock_list);
        if (new_prize_err != -1){ 
            // draws in the board            
            draw_player(my_win, &init_pos, true);
            // increments the number of prizes and elements
            pthread_mutex_lock(ptr_mtx);
            num_prizes++;
            num_elements++;
            pthread_mutex_unlock(ptr_mtx);
        }
        else{
            i--;
            //close(server_sock);
            //exit(-1);
        }

    }


    while(1) {
        
        sleep(TIME_GENERATE_PRIZE);

        if(num_prizes < MAX_PRIZES && num_elements < (WINDOW_SIZE-2)*(WINDOW_SIZE-2)) {
            // Initialize prize; assigns it's representation (1, 2, etc) and an empty board position. 
            init_pos = initialize_bot_prizes(false);

            pthread_rwlock_wrlock(ptr_rwlock_list);
            // Adds the Prizes into the list of clients
            new_prize_err = insert_new_client(init_pos.c, init_pos.x, init_pos.y, init_pos.health, -1); // sock_fd doesn't have a meaning for the prizes
            pthread_rwlock_unlock(ptr_rwlock_list);

            if (new_prize_err != -1){    
                
                // draws in the board            
                draw_player(my_win, &init_pos, true);

                // increments the number of prizes
                pthread_mutex_lock(ptr_mtx);
                num_prizes++;
                num_elements++;
                pthread_mutex_unlock(ptr_mtx);
                

                field_st2all(init_pos, init_pos, 0);
            }
        }

    }

    
}

void* client_thread(void* arg){

    int sock_fd = *(int*)arg;
    int nbytes, err_update, delete_client_err;
    message_t in_msg, out_msg;
    client_list* client;  
    position_t new_play, old_pos;

    while(1){
        nbytes = recv(sock_fd, &in_msg, sizeof(in_msg), 0);
        if(nbytes != sizeof(message_t) || (in_msg.type != ball_mov && in_msg.type != continue_game)){
            break;
        }

        mvwprintw(error_win, 1,1,"\n\n\n\n");
        wrefresh(error_win);

        if(in_msg.type == continue_game) {

            pthread_rwlock_rdlock(ptr_rwlock_list);
            client = search_client(sock_fd);
            pthread_rwlock_unlock(ptr_rwlock_list);

            pthread_rwlock_wrlock(ptr_rwlock_list);
            client->health = INITIAL_HEALTH;
            pthread_rwlock_unlock(ptr_rwlock_list);

            pthread_rwlock_rdlock(ptr_rwlock_list);
            new_play.c = client->c;
            new_play.x = client->x;
            new_play.y = client->y;
            new_play.health = client->health;
            pthread_rwlock_unlock(ptr_rwlock_list);
            
            draw_health(&new_play, 0);

            send_all_field(2, sock_fd);
            field_st2all (new_play, new_play, 2);
        }

        // updates the board
        if (in_msg.type == ball_mov) {
            err_update = update_client(sock_fd, in_msg.direction);
        
            // If an error occurs, returns an error message to the player
            if (err_update == -1){
                out_msg = msg2send(error, UNUSED_CHAR, -1, -1, -1, -1);
                send(sock_fd, &out_msg, sizeof(message_t), 0);
            }
        }
    }

    pthread_rwlock_rdlock(ptr_rwlock_list);
    client = search_client(sock_fd);
    old_pos.c = client->c;
    old_pos.x = client->x;
    old_pos.y = client->y;
    old_pos.health = client->health;
    pthread_rwlock_unlock(ptr_rwlock_list);

    pthread_rwlock_wrlock(ptr_rwlock_list);
    //deletes the client from the list of clients
    delete_client_err = delete_client(sock_fd);
    pthread_rwlock_unlock(ptr_rwlock_list);

    if (delete_client_err == -1){
        mvwprintw(error_win, 1,1,"Error disconnecting client.\n");
        wrefresh(error_win);

    }
    else{
        mvwprintw(error_win, 1,1,"\n\n\n\n");
        wrefresh(error_win);
        //decreases number of players
        pthread_mutex_lock(ptr_mtx);
        num_elements--;
        pthread_mutex_unlock(ptr_mtx);
    }

    field_st2all (old_pos, old_pos, 3);
    close(sock_fd);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int sock_fd;
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock == -1){
		perror("socket: ");
		exit(-1);
	}

	struct sockaddr_in server_address, client_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(SOCK_PORT);
	server_address.sin_addr.s_addr = INADDR_ANY;
    socklen_t client_addr_size = sizeof(struct sockaddr_in);

    int on = 1;
    setsockopt (server_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (on));

	int err = bind(server_sock, (struct sockaddr *)&server_address, sizeof(server_address));
	if(err == -1) {
		perror("bind");
		exit(-1);
	}

    err = listen(server_sock, 10);
    if(err == -1) {
        perror("listen");
        exit(-1);
    }

    // Variables declaration and initialization
    // Local variables
    pthread_t thread_id, thread_id_prizes, thread_id_bots;
    message_t in_msg, out_msg;
    int nbytes, flag_thread;
    position_t init_pos;
    int new_client_err;
    int aux_elements;

    // Global variables
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
        sock_fd = accept(server_sock, (struct sockaddr *) &client_address, &client_addr_size);
        if(sock_fd< 0){
            mvwprintw(error_win, 1,1,"Error while accepting connection to server.\n");
            wrefresh(error_win);
        }
        else {
            mvwprintw(error_win, 1,1,"\n\n\n\n");
            wrefresh(error_win);
            nbytes = recv(sock_fd, &in_msg, sizeof(in_msg), 0);
            if(nbytes == sizeof(message_t) && in_msg.type == conn){
                
                pthread_mutex_lock(ptr_mtx);
                aux_elements = num_elements;
                pthread_mutex_unlock(ptr_mtx);

                if (aux_elements < ((WINDOW_SIZE-2)*(WINDOW_SIZE-2))) {
                    
                    init_pos = initialize_player();     //Initializes the player position: assignes an empty board position and unused letter
                    
                    pthread_rwlock_wrlock(ptr_rwlock_list);
                    new_client_err = insert_new_client(init_pos.c, init_pos.x, init_pos.y, INITIAL_HEALTH, sock_fd);   // adds the player client to the list
                    pthread_rwlock_unlock(ptr_rwlock_list);

                    if (new_client_err != -1){      //if successfully added to the list

                        //increments the number of players
                        pthread_mutex_lock(ptr_mtx);
                        num_elements++; 
                        pthread_mutex_unlock(ptr_mtx);

                        // Draws the new player in the server board
                        draw_player(my_win, &init_pos, true);
                        draw_health(&init_pos, 0);

                        //sends a message to the player containing the assigned, position and character 
                        out_msg = msg2send(ball_info, init_pos.c, init_pos.x, init_pos.y, -1, INITIAL_HEALTH);
                        flag_thread = 1;

                    }
                    else{  //case error allocating memory
                        out_msg = msg2send(error, UNUSED_CHAR, -1, -1, -1, -1);
                        flag_thread = 0;
                    }
                }
                else{ //case exceed number of elements
                    //sends an error message to the player
                    out_msg = msg2send(error, UNUSED_CHAR, -1, -1, -1, -1);
                    flag_thread = 0;
                }
                
                send(sock_fd, &out_msg, sizeof(message_t), 0);
                
                if(flag_thread == 1) {
                    send_all_field(0, sock_fd);
                    // Updates the field for every player
                    field_st2all (init_pos, init_pos, 0);
                    pthread_create(&thread_id, NULL, client_thread,(void*)&sock_fd);
                }
                else {
                    close(sock_fd);
                }
            }
            else{
                perror("Error");
                close(sock_fd);
            }
        }

    }
}
