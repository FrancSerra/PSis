#include "chase.h"

// Create mutex and rwlocks
static pthread_rwlock_t rwlock_list = PTHREAD_RWLOCK_INITIALIZER; // rwlock to access the list
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER; // mutex to access num_elements and num_prizes
pthread_mutex_t mtx_draw = PTHREAD_MUTEX_INITIALIZER; // mutex to access draw_player and draw_health

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

int insert_new_client(client_list* head, char c, int x, int y, int health, int socket_id){
// Function that inserts a new player/bot/prize into the list of clients
// Inputs: pointer to the head node, element character, element position x,y, element health
// Outputs: 1 if successfully inserted, -1 if error

    client_list* new_block = NULL;
    
    new_block = (client_list *) malloc(sizeof(client_list));
    
    if(new_block == NULL){ // check success of memory allocation
        printf("Error allocating memory.\n");
        return -1;
    }

    client_list* last = head->next; // last points to the start of the list

    // insert information in the new block of the list
    new_block->c = c;
    new_block->x = x;
    new_block->y = y;
    new_block->health = health;
    new_block->socket_id = socket_id;

    new_block->next = NULL; // new block is the last one in the list, points to NULL
  
    if (head->next == NULL) { // in case the list is empty, new block is the first one
       head->next = new_block;
       return 1;
    }  

    while (last->next != NULL){ // scroll through the list until the end
        last = last->next;        
    }

    last->next = new_block; // insert the new block at the end of the list
    return 1;
}

int delete_client(client_list* head, int socket_id, WINDOW* win){
// Function that deletes a player from the list of clients and the screen
// Inputs: pointer to the head of the list, socket_id, window
// Outputs: 1 if successfully deleted, -1 if it was not in the list
    
    client_list *temp = head->next, *prev; // stores head of the list

    position_t delete_pos; // information to be deleted
 
    // If head node itself holds the key to be deleted
    if (temp != NULL && temp->socket_id == socket_id) {
        head->next = temp->next; // Changed head
        delete_pos.c = temp->c;
        delete_pos.x = temp->x;
        delete_pos.y = temp->y;
        close(temp->socket_id);

        // delete player and its health from the window
        draw_player(win, &delete_pos, false);
        draw_health(&delete_pos, 1);

        free(temp); // free old head
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
    close(temp->socket_id);

    // delete player and its health from the window 
    draw_player(win, &delete_pos, false);
    draw_health(&delete_pos, 1);

    free(temp); // Free memory
    return 1;   //returns 1 if key was present and deleted
}

client_list* search_position(client_list* head, int x, int y){
// Function that searchs a position in the list
// Inputs: pointer to the head node, position x,y to be searched
// Outputs: node that has the desired position (NULL if not found)

    client_list* temp;

    // Search in the list for the desired position x,y
    for (temp = head->next; temp != NULL; temp = temp->next){
        if (temp->x == x && temp->y == y){
            return temp; // returns the node if found
        }
    }

    return temp; // returns NULL if not found
}

int search_letter(client_list* head, char c){
// Function that searchs a character in the list
// Inputs: pointer to the head node, character to be searched
// Outputs: 1 if found, 0 if not

    // Search in the list for the desired character
    for (client_list* temp = head->next; temp != NULL; temp = temp->next){
        if (temp->c == c){
            return 1; // returns 1 if found (character already assigned)
        }
    }

    return 0; //returns 0 if not found
}

client_list* search_client(client_list* head, int socket_id){
// Function that searchs a client in the list
// Inputs: pointer to the head node, client socket_id
// Outputs: node found (NULL if not found)

    client_list* temp;

    // Search in the list for the desired client, using its socket_id
    for (temp = head->next; temp != NULL; temp = temp->next){
        if (temp->socket_id == socket_id){
            return temp; // returns the node found 
        }
    }

    return temp; // returns NULL if not found
}

int delete_prizes(client_list* head, client_list* prize, WINDOW* win){
// Function that deletes a prize from the list and the screen
// Inputs: pointer to the head node, pointer to the node, window
// Outputs: 1 if found and deleted, -1 if not found

    position_t prize_deleted; // information to be deleted
    
    client_list *temp = head->next, *prev; // Store head node

    prize_deleted.c = prize->c;
    prize_deleted.x = prize->x;
    prize_deleted.y = prize->y;

    // Delete from the screen
    draw_player(win, &prize_deleted, false);

 
    // If head node itself holds the key to be deleted
    if (temp != NULL && temp == prize) {
        head->next = temp->next; // changed head

        free(temp); // free old head
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

    free(temp); // free memory
    return 1;   

}

int health_0(client_list* head, client_list* player, WINDOW* win) {
// Function that checks if the health of a player reached 0 and deletes it if so
// Inputs: pointer to the head node, pointer to the player, window
// Outputs: 1 if its health reached 0, 0 if not

    message_t out_msg;
    position_t new_play; 

    // In case player's health reached 0
    if (player->health == 0) {
        
        new_play.c = player->c;
        new_play.health = player->health;

        draw_health(&new_play, 0);
        out_msg = msg2send(health0, UNUSED_CHAR, -1, -1, -1, -1);
        send(player->socket_id, &out_msg, sizeof(message_t), 0);
        return 1;
    }
    else 
        return 0;
}

int update_client(client_list *head, int socket_id, int direction, WINDOW *win){
// Function that updates players in the list and on the screen
// Inputs: pointer to the head node, client socket_id, direction to move, window
// Outputs: 0 if player and list updated, -1 if player not found

    client_list *player;
    client_list *other_player;
    position_t new_play, old_play;
    position_t new_pos, old_pos;

    pthread_rwlock_rdlock(&rwlock_list);
    // Search player in the list by its socket_id
    player = search_client(head, socket_id); 
    // If not found return -1
    if (player == NULL) {
        pthread_rwlock_unlock(&rwlock_list);
        return -1;
    }

    int x, y, err, is_health0;  // aux variables
    x = player->x;              // player position: x
    y = player->y;              // player position: y

    // Stores the last position
    old_play.c = player->c;
    old_play.x = player->x;
    old_play.y = player->y;
    old_play.health = player->health;
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

    pthread_rwlock_rdlock(&rwlock_list);
    // Checks who is in that landing place
    other_player = search_position(head, x, y);

    // If the new position is empty or if it is its current position
    // if (other_player == NULL || other_player == player){
    if (other_player == NULL){ 
        pthread_rwlock_unlock(&rwlock_list);

        // Cleans the older position
        draw_player(win, &old_play, false);
        // Moves the player to the new position and draws it
        move_client(player, win, x, y);
        // Updates the field for every player

        new_pos.c = old_play.c;
        new_pos.x = x;
        new_pos.y = y;
        new_pos.health = old_play.health;

        field_st2all (head, old_pos, new_pos, 1);
    }
    else if(other_player == player){
        pthread_rwlock_unlock(&rwlock_list);

        new_pos = old_pos;

        field_st2all (head, old_pos, new_pos, 1);
    }
    // If the new position is occupied
    else{ 
        // If the other is a player (position does not change)
        if (isalpha(other_player->c) != 0 && other_player->health > 0){
            pthread_rwlock_unlock(&rwlock_list);

            pthread_rwlock_wrlock(&rwlock_list);
            // Increments moving player health and updates on the screen
            if (player->health < INITIAL_HEALTH){
                    player->health++;
                    new_play.c = player->c;
                    new_play.health = player->health;
                    draw_health(&new_play, 0);
                }

            // Decrements other player health and checks if reached 0
            other_player->health--;
            pthread_rwlock_unlock(&rwlock_list);

            pthread_rwlock_rdlock(&rwlock_list);
            old_pos.health = player->health;
            new_pos.c = other_player->c;
            new_pos.x = other_player->x;
            new_pos.y = other_player->y;
            new_pos.health = other_player->health;
            pthread_rwlock_unlock(&rwlock_list);

            // Updates the field for every player
            field_st2all (head, old_pos, new_pos, 2);

            pthread_rwlock_rdlock(&rwlock_list);
            is_health0 = health_0(head, other_player, win) ;
                if (is_health0) {
                    pthread_rwlock_unlock(&rwlock_list);
                    return 0;
                    }

            // Updates other player's health on the screen
            new_play.c = other_player->c;
            new_play.health = other_player->health;
            pthread_rwlock_unlock(&rwlock_list);

            draw_health(&new_play, 0);
            
            
        }
        // If the other is a prize
        else if (isdigit(other_player->c)){ 
            pthread_rwlock_unlock(&rwlock_list);

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
                draw_health(&new_play, 0);
            }
            // Delete "eaten" prize and decrement number of prizes in the field
            err = delete_prizes(head, other_player, win);
            pthread_rwlock_unlock(&rwlock_list);
            
            if (err != -1) {

                pthread_mutex_lock(&mtx);
                num_prizes --;
                num_elements --;
                pthread_mutex_unlock(&mtx);

                // Updates the server field
                draw_player(win, &old_play, false);
                
                move_client(player, win, x, y);

                new_pos.c = old_pos.c;
                new_pos.x = x;
                new_pos.y = y;
                pthread_rwlock_rdlock(&rwlock_list);
                new_pos.health = player->health;
                pthread_rwlock_unlock(&rwlock_list);
                // Updates the field for every player
                field_st2all (head, old_pos, new_pos, 1);
            }   
        }
        else{
        pthread_rwlock_unlock(&rwlock_list);
        // If the other is a bot, nothing happens (position does not change)
        }
    }

    return 0;
}

client_list* update_bot(client_list *head, client_list* aux, int mod, WINDOW* win) {
// Function that updates bots in the list and on the screen
// Inputs: pointer to the head node, pointer to an aux node, flag direction, window
// Outputs: pointer to the node

    int x,y;
    client_list *other_client;
    position_t new_play, old_play;
    position_t new_pos, old_pos;

    pthread_rwlock_rdlock(&rwlock_list);
    // Search in the list
    for (client_list* temp = aux->next; temp != NULL; temp = temp->next){   
        // If it is a bot
        if (temp->c == BOT_CHAR){

            x = temp->x; // bot position: x
            y = temp->y; // bot position: y

            // Stores the last position
            old_play.c = temp->c;
            old_play.x = temp->x;
            old_play.y = temp->y;
            old_play.health = temp->health;
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

            pthread_rwlock_rdlock(&rwlock_list);
            // Checks who is in that landing place (new position)
            other_client = search_position(head, x, y);

            // If the new position is empty or if it is its current position

            // if (other_client == NULL || other_client == temp){ 
            if (other_client == NULL){ 
                pthread_rwlock_unlock(&rwlock_list);

                // Clean the older position
                draw_player(win, &old_play, false);
                // Move the player to the new position and draw it
                move_client(temp, win, x, y);

                new_pos.c = old_play.c;
                new_pos.x = x;
                new_pos.y = y;
                new_pos.health = old_play.health;

                field_st2all (head, old_pos, new_pos, 1);
            }
            else if (other_client == temp){ 
                pthread_rwlock_unlock(&rwlock_list);

                new_pos = old_pos;

                field_st2all (head, old_pos, new_pos, 1);
            }
            // If the new position is occupied
            else{ 
                // Occupied by a player
                if (isalpha(other_client->c) != 0 && other_client->health > 0){
                    pthread_rwlock_unlock(&rwlock_list);
                    pthread_rwlock_wrlock(&rwlock_list);
                    // Decrements that player's health
                    other_client->health--;
                    pthread_rwlock_unlock(&rwlock_list);

                    pthread_rwlock_rdlock(&rwlock_list);
                    new_pos.c = other_client->c;
                    new_pos.x = other_client->x;
                    new_pos.y = other_client->y;
                    new_pos.health = other_client->health;
                    pthread_rwlock_unlock(&rwlock_list);

                    // Updates the field for every player
                    field_st2all (head, old_pos, new_pos, 2);


                    pthread_rwlock_rdlock(&rwlock_list);
                    int is_health0 = health_0(head, other_client, win);
                    if (is_health0) {
                        pthread_rwlock_unlock(&rwlock_list);
                        return temp;
                    }

                    new_play.c = other_client->c;
                    new_play.health = other_client->health;
                    pthread_rwlock_unlock(&rwlock_list);
                    draw_health(&new_play, 0);
                }
                else{
                    pthread_rwlock_unlock(&rwlock_list);
                    // If the other is a bot or prize, nothing happens
                }
            }   
            return temp; 
        } 
        else {
            pthread_rwlock_unlock(&rwlock_list);
        }
        pthread_rwlock_rdlock(&rwlock_list);
    }
    pthread_rwlock_unlock(&rwlock_list);
    return NULL;
}



// Functions for communications (initialize and messages)

message_t msg2send(msg_type type, char c, int x, int y, int direction, int health) {
// Function that fills in info in the message struct
// Inputs: message type, client char, client position x,y, direction to move, client health
// Outputs: message to be sent

    message_t out_msg;

    out_msg.type = type;
    out_msg.c = c;
    out_msg.x = x;
    out_msg.y = y;
    out_msg.direction = direction;
    out_msg.health = health;

    return out_msg;
}

message_fieldstat_t msg2send_fieldstat(int flag, position_t old_pos, position_t new_pos) {
// Function that fills in info in the message struct (in case of field_status type)
// Inputs: 
// Outputs: message to be sent

    message_fieldstat_t out_msg;

    out_msg.flag = flag;
    out_msg.old_pos = old_pos;
    out_msg.new_pos = new_pos;

    
    return out_msg;
}

position_t initialize_player(client_list* head) {
// Function that initializes the player
// Inputs: pointer to head node
// Outputs: information to be printed (position x,y, character, health)

    srand(time(NULL)); // initialize random number generators

    position_t init_pos; // initial position
    char c = 64; // '@' in ASCII, 65 = 'A'

    client_list* search_pos = NULL;
    int search_lett = 1; // 1 if found (letter used), 0 if not found (can be used)

    // Generate random position x,y
    init_pos.x = (rand()% (WINDOW_SIZE-2 - 1 + 1)) + 1;
    init_pos.y = (rand()% (WINDOW_SIZE-2 - 1 + 1)) + 1;

    pthread_rwlock_rdlock(&rwlock_list);
    search_pos = search_position(head, init_pos.x, init_pos.y); // search position in the list

    while (search_pos != NULL || search_lett == 1){
        // If position found (occupied) generates new position and search again
        if (search_pos != NULL) {
            init_pos.x = (rand()% (WINDOW_SIZE-2 - 1 + 1)) + 1;
            init_pos.y = (rand()% (WINDOW_SIZE-2 - 1 + 1)) + 1;

            search_pos = search_position(head, init_pos.x, init_pos.y);
        }
        // If letter used, move to the next letter in the alphabet and search again
        if (search_lett == 1) {
            c++; 
            search_lett = search_letter(head, c);
        }
    }

    pthread_rwlock_unlock(&rwlock_list);

    // stores the letter and the INITIAL_HEALTH
    init_pos.c = c;
    init_pos.health = INITIAL_HEALTH;
    return init_pos;
}

position_t initialize_bot_prizes(client_list *head, int bot){
// Function that initializes the bots/prizes (similar to the function initialize_player)
// Inputs: pointer to head node, flag bot
// Outputs: information to be printed (position x,y, character, health)

    srand(time(NULL));
    position_t init_pos;

    // Generate a random position and search in the list if that position is occupied
    init_pos.x = (rand() % (WINDOW_SIZE - 2 - 1 + 1)) + 1;
    init_pos.y = (rand() % (WINDOW_SIZE - 2 - 1 + 1)) + 1;

    pthread_rwlock_rdlock(&rwlock_list);

    while (search_position(head, init_pos.x, init_pos.y) != NULL){ 
        init_pos.x = (rand() % (WINDOW_SIZE - 2 - 1 + 1)) + 1;
        init_pos.y = (rand() % (WINDOW_SIZE - 2 - 1 + 1)) + 1;
    }
    pthread_rwlock_unlock(&rwlock_list);

    // If its a bot, stores bot character
    if(bot) {
        init_pos.c = BOT_CHAR;
    }
    // If its a prize, generates a random number between the defined values and stores it
    else {
        init_pos.c = (rand() % (MAX_VALUE_PRIZES - MIN_VALUE_PRIZES + 1)) + MIN_VALUE_PRIZES;
    }
    init_pos.health = -1; // health is not used for bots and prizes (assigned value -1)
    return init_pos;
}

void field_st2all (client_list* head, position_t old_pos, position_t new_pos, int flag_2msg) {
    message_t out_msg;
    message_fieldstat_t out_msg_fieldstat;
    client_list* temp;
    int flag_1msg = 1; // sends only one fieldstat message 

    out_msg = msg2send(field_stat, UNUSED_CHAR, -1, -1, flag_1msg, -1); 
    out_msg_fieldstat = msg2send_fieldstat(flag_2msg, old_pos, new_pos);

    pthread_rwlock_rdlock(&rwlock_list);
    for (temp = head->next; temp != NULL; temp = temp->next) {
        if (temp->socket_id != -1) {
            send(temp->socket_id, &out_msg, sizeof(message_t), 0);
            send(temp->socket_id, &out_msg_fieldstat, sizeof(message_fieldstat_t), 0);
        }
    }
    pthread_rwlock_unlock(&rwlock_list);
    
}

void send_all_field(client_list* head, int flag_1msg, int sock_fd){   
    message_t out_msg;
    message_fieldstat_t out_msg_fieldstat;
    client_list* temp;
    position_t new_pos;
    int aux_elements;

    pthread_mutex_lock(ptr_mtx);
    aux_elements = num_elements;
    pthread_mutex_unlock(ptr_mtx);

    out_msg = msg2send(field_stat, UNUSED_CHAR, -1, -1, flag_1msg, aux_elements); // send number of elements in the health parameter
    send(sock_fd, &out_msg, sizeof(message_t), 0);


    pthread_rwlock_rdlock(&rwlock_list);
    for (temp = head->next; temp != NULL; temp = temp->next) {
        new_pos.c = temp->c;
        new_pos.x = temp->x;
        new_pos.y = temp->y;
        new_pos.health = temp->health;
    
        out_msg_fieldstat = msg2send_fieldstat(0, new_pos, new_pos);
        send(sock_fd, &out_msg_fieldstat, sizeof(message_fieldstat_t), 0);

    }
    pthread_rwlock_unlock(&rwlock_list);
}

void mng_field_status(WINDOW* my_win, message_fieldstat_t msg){

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


// Functions for graphical part (windows and draw players, bots and prizes in the field)

WINDOW* generate_window() {
// Function provided by the Professor

    initscr();		    	/* Start curses mode 		*/
	cbreak();				/* Line buffering disabled	*/
    keypad(stdscr, TRUE);   /* We get F1, F2 etc..		*/
	noecho();			    /* Don't echo() while we do getch */

    if(COLOR == 1){

        if(has_colors() == FALSE){
            endwin();
	    	printf("Your terminal does not support color\n");
	    	exit(1);
	    }

        start_color();			/* Start color 			*/
    
        init_pair(1, COLOR_YELLOW, COLOR_BLACK); // yellow for players
        init_pair(2, COLOR_RED, COLOR_BLACK);    // red for bots
        init_pair(3, COLOR_GREEN, COLOR_BLACK);  // green for prizes
    }

    /* creates a window and draws a border */ 
    WINDOW * my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(my_win, 0 , 0);	
	wrefresh(my_win);
    keypad(my_win, true);

    /* creates a window and draws a border  */
    message_win = newwin(MSG_BOX_HEIGHT, WINDOW_SIZE, WINDOW_SIZE, 0);
    box(message_win, 0 , 0);	
	wrefresh(message_win);

    /* creates a window and draws a border  */
    error_win = newwin(6, WINDOW_SIZE, WINDOW_SIZE+MSG_BOX_HEIGHT, 0);
	wrefresh(error_win);

    return(my_win);
}

void reset_windows(WINDOW* my_win){
    werase(my_win);
    wrefresh (my_win);

    werase(message_win);
    wrefresh (message_win);

    box(my_win, 0 , 0);	
	wrefresh(my_win);
    keypad(my_win, true);

    box(message_win, 0 , 0);	
	wrefresh(message_win);
}

void draw_player(WINDOW *win, position_t * player, int delete){
// Function provided by the Professor

    int ch;

    pthread_mutex_lock(&mtx_draw);

    if(delete){
        ch = player->c;
    }else{
        ch = ' ';
    }
    int p_x = player->x;
    int p_y = player->y;

    if (ch == char_client && COLOR == 1){
        wattron(win, COLOR_PAIR(1));
    }
    else if (ch == BOT_CHAR && COLOR == 1){
        wattron(win, COLOR_PAIR(2));
    }
    else if (ch >= MIN_VALUE_PRIZES && ch <= MAX_VALUE_PRIZES && COLOR == 1){
        wattron(win, COLOR_PAIR(3));
    }
    
    wmove(win, p_y, p_x);
    waddch(win,ch);
    wrefresh(win);

    pthread_mutex_unlock(&mtx_draw);

    if (ch == char_client && COLOR == 1){
        wattroff(win, COLOR_PAIR(1));
    }
    else if (ch == BOT_CHAR && COLOR == 1){
        wattroff(win, COLOR_PAIR(2));
    }
    else if (ch >= MIN_VALUE_PRIZES && ch <= MAX_VALUE_PRIZES && COLOR == 1){
        wattroff(win, COLOR_PAIR(3));
    }

}

void draw_health(position_t * player, int to_do) {
// Function prints the health of the player in the message window
// Inputs: information to be printed, flag to-do, flag connected client
// Outputs: --

    // to_do : 0 - editar player health, 1 - eliminar player health
    int aux = 1;
    int c = 65; // 'A' in ASCII

    pthread_mutex_lock(&mtx_draw);
 
    // Find which player (by its character)
    if (player != NULL) {
        while( c != player->c){
            c++;
            aux++;
        }
    }

    switch (to_do){
    // Edit player's health
    case 0:
        if(COLOR == 1 && player->c == char_client){
            wattron(message_win, COLOR_PAIR(1));
        }

        mvwprintw(message_win, aux,2,"%c:   ", player->c);
        if (player->health >= 0) {
            mvwprintw(message_win, aux,2,"%c: %d", player->c, player->health);
        }
        else {
            mvwprintw(message_win, aux,2,"%c: %d", player->c, 0);
        }
        wrefresh(message_win);

        if(COLOR == 1 && player->c == char_client){
            wattroff(message_win, COLOR_PAIR(1));
        }

        break;
    
    // Deletes player's health, cleans the message window
    case 1:
        mvwprintw(message_win, aux,2,"     ");
        wrefresh(message_win);
        break;
    
    default:
        break;
    }

    pthread_mutex_unlock(&mtx_draw);

}

void move_client (client_list* client, WINDOW* win, int x, int y){
// Function that moves the player/bots and draws the new position
// Inputs: pointer to the node to be updated, window, position x,y
// Outputs: -- 

    position_t new_play;

    pthread_rwlock_wrlock(&rwlock_list);
    // Updates position
    client->x = x;
    client->y = y;
    pthread_rwlock_unlock(&rwlock_list);

    pthread_rwlock_rdlock(&rwlock_list);
    // We have to pass a struct
    new_play.c = client->c;
    new_play.x = client->x;
    new_play.y = client->y;

    // Draws the new position
    draw_player(win, &new_play, true);
    pthread_rwlock_unlock(&rwlock_list);
}


// Handler functions

void sig_handler(int signum){
  mvwprintw(error_win, 1,1,"Key not pressed.\n Game over!\n");
  wrefresh(error_win);
  free(field);
  close(client_sock);
  exit(-1);
}
