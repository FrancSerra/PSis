#include "fcn_server.h"

// Mutex and read-write lock initialization
static pthread_rwlock_t rwlock_list = PTHREAD_RWLOCK_INITIALIZER;   // RW lock to access the client_list
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;             // Mutex to access num_elements and num_prizes

pthread_rwlock_t* ptr_rwlock_list = &rwlock_list;
pthread_mutex_t* ptr_mtx = &mtx;

// Functions for lists and updates

client_list* create_head_client_list(){
// Function that creates the head node of the list of players, bots and prizes
// Inputs: --
// Outputs: pointer to the head node

    client_list* head = NULL;
    head = (client_list*) malloc(sizeof(client_list));  // allocate memory

    if(head == NULL){ // check success of memory allocation
        printf("Error allocating memory.\n");
        exit(-1);
    }

    head->next=NULL; // nothing yet connected to the head node 

    return head;
}

int insert_new_client(char c, int x, int y, int health, int socket_id){
// Function that inserts a new player/bot/prize into the list of clients
// Inputs: element character, element position x, y, element health, client socket id
// Outputs: 1 if successfully inserted, -1 if error

    client_list* new_block = NULL;
    
    // Allocates memory
    new_block = (client_list *) malloc(sizeof(client_list));
    
    // Checks success of memory allocation
    if(new_block == NULL){ 
        printf("Error allocating memory.\n");
        return -1;
    }

    // Last points to the start of the list
    client_list* last = head->next; 

    // Inserts information in the new block of the list
    new_block->c = c;
    new_block->x = x;
    new_block->y = y;
    new_block->health = health;
    new_block->socket_id = socket_id;

    // New block is the last one in the list, points to NULL
    new_block->next = NULL; 
    
    // In case the list is empty, new block is the first one
    if (head->next == NULL) { 
       head->next = new_block;
       return 1;
    }  

    // Searchs the list until the end
    while (last->next != NULL){ 
        last = last->next;        
    }

    // Inserts the new block at the end of the list
    last->next = new_block; 
    return 1;
}

int delete_client(int socket_id){
// Function that deletes a player from the list of clients and the screen
// Inputs: socket id of the client to be deleted
// Outputs: 1 if successfully deleted, -1 if it was not in the list
    
    // Stores head of the list
    client_list *temp = head->next, *prev; 

     // Information to be deleted
    position_t delete_pos;
 
    // If head node itself holds the key to be deleted
    if (temp != NULL && temp->socket_id == socket_id) {
        head->next = temp->next; // Changed head
        delete_pos.c = temp->c;
        delete_pos.x = temp->x;
        delete_pos.y = temp->y;
        close(temp->socket_id); // Closes the client socket

        // Delete player and its health from the window
        draw_player(my_win, &delete_pos, false);
        draw_health(&delete_pos, 1);

        // Free old head
        free(temp); 
        return 1;   
    }
 
    // Search for the key to be deleted, keep track of the previous node as we need to change 'prev->next'
    while (temp != NULL && temp->socket_id != socket_id) {
        prev = temp;
        temp = temp->next;
    }
 
    // If key was not present in linked list
    if (temp == NULL)
        return -1;
 
    // Unlink the node from linked list
    prev->next = temp->next;
    
    delete_pos.c = temp->c;
    delete_pos.x = temp->x;
    delete_pos.y = temp->y;

    // Closes the client socket
    close(temp->socket_id);

    // Deletes player and its health from the window 
    draw_player(my_win, &delete_pos, false);
    draw_health(&delete_pos, 1);

    // Free memory
    // Returns 1 if key was present and deleted
    free(temp); 
    return 1;   
}

void delete_all_list() {
// Function that deletes all list and closes all client sockets 

    client_list *current, *next;

    current = head->next;

    while (current != NULL){
        next = current->next;
        if (current->socket_id != -1)
            close(current->socket_id);
        free(current);
        current = next;
    }
    free(head);
}

client_list* search_position(int x, int y){
// Function that searchs a position in the list
// Inputs: position x,y to be searched
// Outputs: node that has the desired position (NULL if not found)

    client_list* temp;

    // Search in the list for the desired position x,y
    for (temp = head->next; temp != NULL; temp = temp->next){
        if (temp->x == x && temp->y == y){
            // Returns the node if found
            return temp; 
        }
    }

    // Returns NULL if not found
    return temp; 
}

int search_letter(char c){
// Function that searchs a character in the list
// Inputs: character to be searched
// Outputs: 1 if found, 0 if not

    // Search in the list for the desired character
    for (client_list* temp = head->next; temp != NULL; temp = temp->next){
        if (temp->c == c){
            // Returns 1 if found (character already assigned)
            return 1; 
        }
    }

    // Returns 0 if not found
    return 0; 
}

client_list* search_client(int socket_id){
// Function that searchs a client in the list
// Inputs: client socket_id to be searched
// Outputs: node found (NULL if not found)

    client_list* temp;

    // Search in the list for the desired client, using its socket_id
    for (temp = head->next; temp != NULL; temp = temp->next){
        if (temp->socket_id == socket_id){
            // Returns the node found 
            return temp; 
        }
    }

    // Returns NULL if not found
    return temp; 
}

int delete_prizes(client_list* prize){
// Function that deletes a prize from the list and the screen
// Inputs: pointer to the node
// Outputs: 1 if found and deleted, -1 if not found

    // Information to be deleted
    position_t prize_deleted; 
    
    // Stores the head node
    client_list *temp = head->next, *prev; 

    prize_deleted.c = prize->c;
    prize_deleted.x = prize->x;
    prize_deleted.y = prize->y;

    // Deletes from the screen
    draw_player(my_win, &prize_deleted, false);

 
    // If head node itself holds the key to be deleted
    if (temp != NULL && temp == prize) {
        // Change head
        head->next = temp->next; 
        
        // Free old head
        free(temp); 
        return 1;   
    }
 
    // Search for the key to be deleted, keep track of the previous node as we need to change 'prev->next'
    while (temp != NULL && temp != prize) {
        prev = temp;
        temp = temp->next;
    }
 
    // If key was not present in linked list
    if (temp == NULL)
        return -1; 
 
    // Unlink the node from linked list
    prev->next = temp->next;

    // Free memory
    free(temp); 
    return 1;   

}

int health_0(client_list* player) {
// Function that checks if the health of a player reached 0 and sends a health0 message to that player
// Inputs: pointer to the player
// Outputs: 1 if its health reached 0, 0 if not

    message_t out_msg;
    position_t new_play; 

    // In case player's health reached 0
    if (player->health == 0) {
        
        new_play.c = player->c;
        new_play.health = player->health;

        // Draws its health on the server screen
        draw_health(&new_play, 0);

        // Sends a health0 message to that client
        out_msg = msg2send(health0, new_play.c, -1, -1, -1, new_play.health);
        send(player->socket_id, &out_msg, sizeof(message_t), 0);
        return 1;
    }
    else 
        return 0;
}

int update_client(int socket_id, int direction){
// Function that updates players in the list and on the screen
// Inputs: client socket_id, direction to move
// Outputs: 0 if player and list updated, -1 if player not found

    client_list *player;
    client_list *other_player;
    position_t new_play, old_play;
    position_t new_pos, old_pos;

    // Read lock to search the client in the list by its socket_id
    pthread_rwlock_rdlock(&rwlock_list);
    player = search_client(socket_id); 
    // If not found return -1
    if (player == NULL) {
        // Read unlock
        pthread_rwlock_unlock(&rwlock_list);
        return -1;
    }

    int x, y, err, is_health0;  // Aux variables
    x = player->x;              // Player position: x
    y = player->y;              // Player position: y

    // Stores the last position
    old_play.c = player->c;
    old_play.x = player->x;
    old_play.y = player->y;
    old_play.health = player->health;
    // Read unlock
    pthread_rwlock_unlock(&rwlock_list);

    old_pos = old_play;

    // Computes where it will land following the given direction
    if (direction == KEY_UP){
        if (y != 1){
            y--;
        }
    }
    if (direction == KEY_DOWN){
        if (y != WINDOW_SIZE - 2){
            y++;
        }
    }
    if (direction == KEY_LEFT){
        if (x != 1){
            x--;
        }
    }
    if (direction == KEY_RIGHT) {
        if (x != WINDOW_SIZE - 2){
            x++;
        }
    }

    // Read lock to search position in the list
    pthread_rwlock_rdlock(&rwlock_list);
    // Checks who is in that landing place
    other_player = search_position(x, y);

    // If the new position is empty
    if (other_player == NULL){ 
        pthread_rwlock_unlock(&rwlock_list);

        // Cleans the older position
        draw_player(my_win, &old_play, false);
        // Moves the player to the new position and draws it
        move_client(player, x, y);
        
        // Updates the field for every player
        new_pos.c = old_play.c;
        new_pos.x = x;
        new_pos.y = y;
        new_pos.health = old_play.health;

        field_st2all (old_pos, new_pos, 1);
    }
    // If the new position is its current position (case of borders)
    else if(other_player == player){
        // Read unlock
        pthread_rwlock_unlock(&rwlock_list);

        // Updates the field for every player
        new_pos = old_pos;
        field_st2all (old_pos, new_pos, 1);
    }
    // If the new position is occupied
    else{ 
        // If the other is a player (position does not change)
        if (isalpha(other_player->c) != 0 && other_player->health > 0){
            // Read unlock
            pthread_rwlock_unlock(&rwlock_list);

            // Write lock to write in the list
            pthread_rwlock_wrlock(&rwlock_list);
            // Increments moving player health and updates on the screen
            if (player->health < INITIAL_HEALTH){
                    player->health++;
                    new_play.c = player->c;
                    new_play.health = player->health;
                    draw_health(&new_play, 0);
                }

            // Decrements other player health
            other_player->health--;
            // Write unlock
            pthread_rwlock_unlock(&rwlock_list);

            // Read lock
            pthread_rwlock_rdlock(&rwlock_list);
            old_pos.health = player->health;
            new_pos.c = other_player->c;
            new_pos.x = other_player->x;
            new_pos.y = other_player->y;
            new_pos.health = other_player->health;
            // Read unlock
            pthread_rwlock_unlock(&rwlock_list);

            // Updates the field for every player
            field_st2all (old_pos, new_pos, 2);

            // Read lock
            pthread_rwlock_rdlock(&rwlock_list);
            // Checks if other players' health reached 0
            is_health0 = health_0(other_player) ;
                if (is_health0) {
                    // Read unlock
                    pthread_rwlock_unlock(&rwlock_list);
                    return 0;
                    }

            // Updates other player's health on the screen
            new_play.c = other_player->c;
            new_play.health = other_player->health;
            // Read unlock
            pthread_rwlock_unlock(&rwlock_list);

            draw_health(&new_play, 0);
            
            
        }
        // If the other is a prize
        else if (isdigit(other_player->c)){ 
            // Read unlock
            pthread_rwlock_unlock(&rwlock_list);

            // Write lock to write in the list
            pthread_rwlock_wrlock(&rwlock_list);
            // Sums the prize to player's health
            if (player->health < INITIAL_HEALTH){

                int health = player->health;

                int prize = other_player->c - ZERO_ASCII;
                player->health = health + prize; 

                // Player's health must not exceed INITIAL_HEALTH
                if (player->health > INITIAL_HEALTH)
                    player->health = INITIAL_HEALTH;

                new_play.c = player->c;
                new_play.health = player->health;

                // Prints the new health
                draw_health(&new_play, 0);
            }
            // Deletes "eaten" prize and decrement number of prizes and elements in the field
            err = delete_prizes(other_player);

            // Write unlock
            pthread_rwlock_unlock(&rwlock_list);
            
            if (err != -1) {
                // Mutex lock
                pthread_mutex_lock(&mtx);
                num_prizes --;
                num_elements --;
                // Mutex unlock
                pthread_mutex_unlock(&mtx);

                // Deletes the old position
                draw_player(my_win, &old_play, false);

                // Moves the player
                move_client(player, x, y);

                new_pos.c = old_pos.c;
                new_pos.x = x;
                new_pos.y = y;

                // Read lock to read from the list
                pthread_rwlock_rdlock(&rwlock_list);
                new_pos.health = player->health;
                // Read unlock
                pthread_rwlock_unlock(&rwlock_list);

                // Updates the field for every player
                field_st2all (old_pos, new_pos, 1);
            }   
        }
        else{
            // Read unlock
            pthread_rwlock_unlock(&rwlock_list);
            // If the other is a bot, nothing happens (position does not change)
        }
    }

    return 0;
}

client_list* update_bot(client_list* aux, int mod) {
// Function that updates bots in the list and on the screen
// Inputs: pointer to an aux node (previous bot), flag direction
// Outputs: pointer to the node

    int x,y;
    client_list *other_client;
    position_t new_play, old_play;
    position_t new_pos, old_pos;

    // Read lock
    pthread_rwlock_rdlock(&rwlock_list);

    // Search in the list
    for (client_list* temp = aux->next; temp != NULL; temp = temp->next){   
        // If it is a bot
        if (temp->c == BOT_CHAR){

            x = temp->x; // Bot position: x
            y = temp->y; // Bot position: y

            // Stores the last position
            old_play.c = temp->c;
            old_play.x = temp->x;
            old_play.y = temp->y;
            old_play.health = temp->health;

            // Read unlock
            pthread_rwlock_unlock(&rwlock_list);

            old_pos = old_play;

            // Moves up           
            if (mod == 1){
                if (y != 1){
                    y--;
                }
            }
            // Moves down
            if (mod == 2){
                if (y != WINDOW_SIZE - 2){
                    y++;
                }
            }
            // Moves to the left
            if (mod == 3){
                if (x != 1){
                    x--;
                }
            }
            // Moves to the right
            if (mod == 4) {
                if (x != WINDOW_SIZE - 2){
                    x++;
                }
            }

            // Read lock
            pthread_rwlock_rdlock(&rwlock_list);

            // Checks who is in that landing place (new position)
            other_client = search_position(x, y);

            // If the new position is empty 
            if (other_client == NULL){ 
                // Read unlock
                pthread_rwlock_unlock(&rwlock_list);

                // Clean the older position
                draw_player(my_win, &old_play, false);
                // Move the player to the new position and draw it
                move_client(temp, x, y);

                new_pos.c = old_play.c;
                new_pos.x = x;
                new_pos.y = y;
                new_pos.health = old_play.health;

                // Updates the field for every player
                field_st2all (old_pos, new_pos, 1);
            }
            else if (other_client == temp){ 
                // Read unlock
                pthread_rwlock_unlock(&rwlock_list);

                new_pos = old_pos;

                // Updates the field for every player
                field_st2all (old_pos, new_pos, 1);
            }
            // If the new position is occupied
            else{ 
                // Occupied by a player
                if (isalpha(other_client->c) != 0 && other_client->health > 0){
                    // Read unlock
                    pthread_rwlock_unlock(&rwlock_list);

                    // Write lock
                    pthread_rwlock_wrlock(&rwlock_list);
                    // Decrements that player's health
                    other_client->health--;
                    // Write unlock
                    pthread_rwlock_unlock(&rwlock_list);

                    // Read lock
                    pthread_rwlock_rdlock(&rwlock_list);
                    new_pos.c = other_client->c;
                    new_pos.x = other_client->x;
                    new_pos.y = other_client->y;
                    new_pos.health = other_client->health;
                    // Read unlock
                    pthread_rwlock_unlock(&rwlock_list);

                    // Updates the field for every player
                    field_st2all (old_pos, new_pos, 2);

                    // Read lock
                    pthread_rwlock_rdlock(&rwlock_list);
                    // Checks if other player's health reached 0
                    int is_health0 = health_0(other_client);
                    if (is_health0) {
                        // Read unlock
                        pthread_rwlock_unlock(&rwlock_list);
                        return temp;
                    }
                    
                    new_play.c = other_client->c;
                    new_play.health = other_client->health;

                    // Read unlock
                    pthread_rwlock_unlock(&rwlock_list);
                    // Updates other player's health on the screen
                    draw_health(&new_play, 0);
                }
                else{
                    // Read unlock
                    pthread_rwlock_unlock(&rwlock_list);
                    // If the other is a bot or prize, nothing happens
                }
            }   
            return temp; 
        } 
        else {
            // Read unlock
            pthread_rwlock_unlock(&rwlock_list);
        }
        // Read lock
        pthread_rwlock_rdlock(&rwlock_list);
    }
    // Read unlock
    pthread_rwlock_unlock(&rwlock_list);
    return NULL;
}

position_t initialize_player() {
// Function that initializes the player
// Outputs: information to be printed (position x, y, character, health)

    // Initialize random number generators
    srand(time(NULL)); 

    // Initial position
    position_t init_pos; 
    char c = 64; // '@' in ASCII, 65 = 'A'

    client_list* search_pos = NULL;
    int search_lett = 1; // 1 if found (letter used), 0 if not found (can be used)

    // Generate random position (x,y)
    init_pos.x = (rand()% (WINDOW_SIZE-2 - 1 + 1)) + 1;
    init_pos.y = (rand()% (WINDOW_SIZE-2 - 1 + 1)) + 1;

    // Read lock to search position in the list
    pthread_rwlock_rdlock(&rwlock_list);
    search_pos = search_position(init_pos.x, init_pos.y); 

    while (search_pos != NULL || search_lett == 1){
        // If position found (occupied) generates new position and search again
        if (search_pos != NULL) {
            init_pos.x = (rand()% (WINDOW_SIZE-2 - 1 + 1)) + 1;
            init_pos.y = (rand()% (WINDOW_SIZE-2 - 1 + 1)) + 1;

            search_pos = search_position(init_pos.x, init_pos.y);
        }
        // If letter used, move to the next letter in the alphabet and search again
        if (search_lett == 1) {
            c++; 
            search_lett = search_letter(c);
        }
    }

    // Read unlock
    pthread_rwlock_unlock(&rwlock_list);

    // Stores the letter and the INITIAL_HEALTH
    init_pos.c = c;
    init_pos.health = INITIAL_HEALTH;
    return init_pos;
}

position_t initialize_bot_prizes(int bot){
// Function that initializes the bots/prizes (similar to the function initialize_player)
// Inputs: flag bot
// Outputs: information to be printed (position x, y, character, health)

    srand(time(NULL));
    position_t init_pos;

    // Generate a random position and search in the list if that position is occupied
    init_pos.x = (rand() % (WINDOW_SIZE - 2 - 1 + 1)) + 1;
    init_pos.y = (rand() % (WINDOW_SIZE - 2 - 1 + 1)) + 1;

    // Read lock to search position in the list
    pthread_rwlock_rdlock(&rwlock_list);

    // Searchs until it founds an empty position
    while (search_position(init_pos.x, init_pos.y) != NULL){ 
        init_pos.x = (rand() % (WINDOW_SIZE - 2 - 1 + 1)) + 1;
        init_pos.y = (rand() % (WINDOW_SIZE - 2 - 1 + 1)) + 1;
    }
    // Read unlock
    pthread_rwlock_unlock(&rwlock_list);

    // If it is a bot, stores bot character
    if(bot) {
        init_pos.c = BOT_CHAR;
    }
    // If it is a prize, generates a random number between the defined values and stores it
    else {
        init_pos.c = (rand() % (MAX_VALUE_PRIZES - MIN_VALUE_PRIZES + 1)) + MIN_VALUE_PRIZES;
    }

    // Health is not used for bots and prizes (assigned value -1)
    init_pos.health = -1; 
    return init_pos;
}


// Functions for communications

void field_st2all (position_t old_pos, position_t new_pos, int flag_2msg) {
// Function that sends a field_status message to all clients
// Inputs: old and new position of type position_t, flag of the second message that tells the clients which type of update it is

    message_t out_msg;
    message_fieldstat_t out_msg_fieldstat;
    client_list* temp;

    // Sends only one message after the first one
    int flag_1msg = 1; 

    // Fills the messages structures
    // Sends the flag in the field 'directions' of the first message
    out_msg = msg2send(field_stat, UNUSED_CHAR, -1, -1, flag_1msg, -1); 
    out_msg_fieldstat = msg2send_fieldstat(flag_2msg, old_pos, new_pos);

    // Read lock to search the list
    pthread_rwlock_rdlock(&rwlock_list);
    for (temp = head->next; temp != NULL; temp = temp->next) {
        if (temp->health > 0 && temp->socket_id != -1) {

            // Sends the field_status message to all clients that do not have health = 0
            send(temp->socket_id, &out_msg, sizeof(message_t), 0);
            send(temp->socket_id, &out_msg_fieldstat, sizeof(message_fieldstat_t), 0);
        }
    }

    // Read unlock
    pthread_rwlock_unlock(&rwlock_list);
    
}

void send_all_field(int flag_1msg, int sock_fd){
// Function that send all field to one client 
// Inputs: flag of the first message that tells the client it will receive more than one message after the first one

    message_t out_msg;
    message_fieldstat_t out_msg_fieldstat;
    client_list* temp;
    position_t new_pos;
    int aux_elements;

    // Mutex lock
    pthread_mutex_lock(ptr_mtx);
    aux_elements = num_elements;
    // Mutex unlock
    pthread_mutex_unlock(ptr_mtx);

    // Fills the structures
    // Sends the flag of the first message in the field 'directions' of the first message
    //      that tells the client it will receive all board update, i.e. more than one message after the first one
    // Sends the num_elements in the field 'health' of the first message
    //      that tells the client how many messages it will receive after the first one        
    out_msg = msg2send(field_stat, UNUSED_CHAR, -1, -1, flag_1msg, aux_elements);
    send(sock_fd, &out_msg, sizeof(message_t), 0);

    // Read lock to access the list
    pthread_rwlock_rdlock(&rwlock_list);
    for (temp = head->next; temp != NULL; temp = temp->next) {
        new_pos.c = temp->c;
        new_pos.x = temp->x;
        new_pos.y = temp->y;
        new_pos.health = temp->health;

        // Sends to one client the information of the entire list in num_elements messages 
        out_msg_fieldstat = msg2send_fieldstat(0, new_pos, new_pos);
        send(sock_fd, &out_msg_fieldstat, sizeof(message_fieldstat_t), 0);

    }
    // Read unlock
    pthread_rwlock_unlock(&rwlock_list);
}


// Functions for graphics 

void move_client (client_list* client, int x, int y){
// Function that moves the player/bots and draws the new position
// Inputs: pointer to the node to be updated, position x, y

    position_t new_play;

    // Write lock
    pthread_rwlock_wrlock(&rwlock_list);

    // Updates position
    client->x = x;
    client->y = y;
    
    // Write unlock
    pthread_rwlock_unlock(&rwlock_list);

    // Read lock
    pthread_rwlock_rdlock(&rwlock_list);

    // We have to pass a struct
    new_play.c = client->c;
    new_play.x = client->x;
    new_play.y = client->y;

    // Draws the new position
    draw_player(my_win, &new_play, true);

    // Read unlock
    pthread_rwlock_unlock(&rwlock_list);
}

// Handler functions

void interupt_handler_server(int s){
    // Function that is called when a Ctrl+C is detected

    // Prints the information, closes the socket and terminates the program
    mvwprintw(error_win, 1,1,"Caught Ctrl+C!\n\n\n");
    wrefresh(error_win);

    // RW lock
    pthread_rwlock_wrlock(ptr_rwlock_list);
    // Deletes all list of elements
    delete_all_list(head);
    // RW unlock
    pthread_rwlock_unlock(ptr_rwlock_list);

    close(server_sock);

    exit(-1); 
}
