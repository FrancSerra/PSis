#include "chase.h"

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

int insert_new_client(client_list* head, int pid, char c, int x, int y, int health){
// Function that inserts a new player/bot/prize into the list of clients
// Inputs: pointer to the head node, client PID, element character, element position x,y, element health
// Outputs: 1 if successfully inserted, -1 if error

    client_list* new_block = NULL;
    
    new_block = (client_list *) malloc(sizeof(client_list));
    
    if(new_block == NULL){ // check success of memory allocation
        printf("Error allocating memory.\n");
        return -1;
    }

    client_list* last = head->next; // last points to the start of the list

    // insert information in the new block of the list
    new_block->pid = pid;
    new_block->c = c;
    new_block->x = x;
    new_block->y = y;
    new_block->health = health;

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

int delete_client(client_list* head, int pid, WINDOW* win){
// Function that deletes a player from the list of clients and the screen
// Inputs: pointer to the head of the list, player PID, window
// Outputs: 1 if successfully deleted, -1 if it was not in the list

    client_list *temp = head->next, *prev; // stores head of the list

    position_t delete_pos; // information to be deleted
 
    // If head node itself holds the key to be deleted
    if (temp != NULL && temp->pid == pid) {
        head->next = temp->next; // Changed head
        delete_pos.c = temp->c;
        delete_pos.x = temp->x;
        delete_pos.y = temp->y;

        // delete player and its health from the window
        draw_player(win, &delete_pos, false);
        draw_health(&delete_pos, 2, false);

        free(temp); // free old head
        return 1;   
    }
 
    // Search for the key to be deleted, keep track of the previous node as we need to change 'prev->next'
    while (temp != NULL && temp->pid != pid) {
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

    // delete player and its health from the window 
    draw_player(win, &delete_pos, false);
    draw_health(&delete_pos, 2, false);

    free(temp); // Free memory
    return 1;   //returns 1 if key was present and deleted
}

void print_client_list(client_list* node){
// Function that prints the list (used for debug)
// Inputs: pointer to the node
// Outputs: --

    node = node->next;

    // Print information and move forward
    while (node != NULL){
       printf("PID: %d\n Letter: %c\n Pos_x: %d\n Pos_y: %d\n Health: %d\n", node->pid, node->c, node->x ,node->y, node->health);
       node = node->next;
    }
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

client_list* search_client(client_list* head, int pid){
// Function that searchs a client in the list
// Inputs: pointer to the head node, client PID
// Outputs: node found (NULL if not found)

    client_list* temp;

    // Search in the list for the desired client, using its PID
    for (temp = head->next; temp != NULL; temp = temp->next){
        if (temp->pid == pid){
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
// Outputs: 1 if its health reached 0 and it was deleted, 0 if not

    int delete_client_err;

    // In case player's health reached 0
    if (player->health == 0) {
        delete_client_err = delete_client(head, player->pid, win); // deletes it from the list and the screen
        
        // In case it was not in the list
        if (delete_client_err == -1) {
            printf("This client was not yet connected.\n");
            return 0;
            }
        // In case it was succefully deleted
        else {
            num_players --; // decrease total number of players 
            return 1;
        }
    }
    else 
        return 0;
}

int update_client(client_list *head, int pid, int direction, WINDOW *win){
// Function that updates players in the list and on the screen
// Inputs: pointer to the head node, client PID, direction to move, window
// Outputs: 0 if player and list updated, -1 if player not found

    client_list *player;
    client_list *other_player;
    position_t new_play, old_play;

    // Search player in the list by its PID
    player = search_client(head, pid); 
    // If not found return -1
    if (player == NULL)
        return -1;

    int x, y, err, is_health0;  // aux variables
    x = player->x;              // player position: x
    y = player->y;              // player position: y

    // Stores the last position
    old_play.c = player->c;
    old_play.x = player->x;
    old_play.y = player->y;

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

    // Checks who is in that landing place
    other_player = search_position(head, x, y);

    // If the new position is empty or if it is its current position
    if (other_player == NULL || other_player == player){ 

        // Cleans the older position
        draw_player(win, &old_play, false);
        // Moves the player to the new position and draws it
        move_client(player, win, x, y);
    }
    // If the new position is occupied
    else{ 

        // If the other is a player (position does not change)
        if (isalpha(other_player->c) != 0){
            
            // Increments moving player health and updates on the screen
            if (player->health < INITIAL_HEALTH){
                player->health++;
                new_play.c = player->c;
                new_play.health = player->health;
                draw_health(&new_play, 1, false);
            }

            // Decrements other player health and checks if reached 0
            other_player->health--;
            is_health0 = health_0(head, other_player, win) ;
            if (is_health0) {
                return 0;
            }

            // Updates other player's health on the screen
            new_play.c = other_player->c;
            new_play.health = other_player->health;
            draw_health(&new_play, 1, false);

            
        }
        // If the other is a prize
        else if (isdigit(other_player->c)){ 

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
                draw_health(&new_play, 1, false);
            }
            // Delete "eaten" prize and decrement number of prizes in the field
            err = delete_prizes(head, other_player, win);
            num_prizes --;
            if (err != -1) {
                // Updates the field
                draw_player(win, &old_play, false);
                move_client(player, win, x, y);
            }   
        }
        // If the other is a bot, nothing happens (posiiton does not change)
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

    // Search in the list
    for (client_list* temp = aux->next; temp != NULL; temp = temp->next)
    {   
        // If it is a bot
        if (temp->c == BOT_CHAR){

            x = temp->x; // bot position: x
            y = temp->y; // bot position: y

            // Stores the last position
            old_play.c = temp->c;
            old_play.x = temp->x;
            old_play.y = temp->y;

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

            // Checks who is in that landing place (new position)
            other_client = search_position(head, x, y);

            // If the new position is empty or if it is its current position
            if (other_client == NULL || other_client == temp){ 

                // Clean the older position
                draw_player(win, &old_play, false);
                // Move the player to the new position and draw it
                move_client(temp, win, x, y);
            }
            // If the new position is occupied
            else{ 
                // Occupied by a player
                if (isalpha(other_client->c) != 0){

                    // Decrements that player's health
                    other_client->health--;
                    int is_health0 = health_0(head, other_client, win);
                    if (is_health0) {
                        return temp;
                    }

                    new_play.c = other_client->c;
                    new_play.health = other_client->health;
                    draw_health(&new_play, 1, false);
                }
                // If the other is a bot or prize, nothing happens
            }   
            return temp; 
        }
    }
    return NULL;
}


// Functions to send message of type field_status 

char* field2msg(client_list* head){
// Function that stores in a string the client list information to be sent in a message (type field_status)
// Inputs: pointer to the head node
// Outputs: message string

    char* msg_result = (char *) malloc(sizeof(char)*(BUFFER_SIZE)+1); 
    char* msg = (char *) malloc(sizeof(char)*(BUFFER_SIZE)+1); 

    // Check success of memory allocation
    if(msg_result == NULL || msg == NULL){ 
        printf("Error allocating memory.\n");
        return NULL;
    }

    char* delim;

    // Create the delimiter that will be sent inside the message
    delim = numToASCII(DELIM);    

    for(client_list* temp = head->next; temp != NULL; temp = temp->next) {
        
        sprintf(msg,"%s","n"); // adds a flag
        strcat(msg,delim);
        strcat(msg_result,msg);
        sprintf(msg,"%d",temp->x); // adds position x
        strcat(msg,delim);
        strcat(msg_result,msg);
        sprintf(msg,"%d",temp->y); // adds position y
        strcat(msg,delim);
        strcat(msg_result,msg);
        sprintf(msg,"%d",temp->c); // adds character
        strcat(msg,delim);
        strcat(msg_result,msg);
        sprintf(msg,"%d",temp->health); // adds health
        strcat(msg,delim);
        strcat(msg_result,msg);

    }

    free(msg); // free allocated memory
    free(delim); // free allocated memory

    return msg_result;
}

char *numToASCII(int num) { 
// Function that creates the string that will delimit the parameters in the message to be sent
// Inputs: ASCII code of the delimiter
// Outputs: delimiter string

    char *string =(char*) malloc(sizeof(char)*BUFFER_SIZE);

    // Check success of memory allocation
    if (string == NULL){
        printf("Error allocating memory\n");
        return NULL;
    }

    string[0] = num;
    string[1] = 0;
    return string;
}

position_t* decode_msg_field(int len, char str[BUFFER_SIZE], WINDOW* win){
// Function that decodes the message sent in the field_status reply
// Inputs: number of elements in the client list, received string message, window
// Outputs: array of structs position_t with all info about the elements in the field

    int flag_x, flag_y, flag_health, letter;
    char letter_c;

    char* delim;
    delim = numToASCII(DELIM);

    position_t* field = (position_t *) malloc(sizeof(position_t)*(MAX_PLAYERS+MAX_BOTS+MAX_PRIZES)+1);
    
    // Check success of memory allocation
    if (field == NULL){
        printf("Error allocating memory\n");
        return NULL;
    }

    // For all elements in the string
    for(int i=0; i<len;i++){

        // Decodes the string
        if (i==0){
            strtok(str, delim);
        }
        else{       
            strtok(NULL, delim);
        }

        flag_x = atoi(strtok(NULL, delim));
        flag_y = atoi(strtok(NULL, delim));
        letter = atoi(strtok(NULL, delim));
        letter_c = letter;
        flag_health = atoi(strtok(NULL, delim));

        // Stores the information
        field[i].c = letter_c;
        field[i].x = flag_x;
        field[i].y = flag_y;
        field[i].health = flag_health;

        // Updates the field
        draw_player(win, &field[i], true);
        if(field[i].health != -1){
            draw_health(&field[i], 1, false); 
        }

    }

    free(delim); // free allocated memory

    return field;

}


// Functions for communications (initialize and messages)

message_t msg2send(msg_type type, int pid, char c, int x, int y, long int direction, int health) {
// Function that fills in info in the message struct
// Inputs: message type, client PID, client char, client position x,y, direction to move, client health
// Outputs: message to be sent

    message_t out_msg;

    out_msg.type = type;
    out_msg.pid = pid;
    out_msg.c = c;
    out_msg.x = x;
    out_msg.y = y;
    out_msg.direction = direction;
    out_msg.health = health;

    return out_msg;
}

message_ballmov_t msg2send_ballmov(msg_type type, int num_elem, char str[BUFFER_SIZE]) {
// Function that fills in info in the message struct (in case of field_status type)
// Inputs: message type, total number of elements in the list, string
// Outputs: message to be sent

    message_ballmov_t out_msg;

    out_msg.type = type;
    out_msg.num_elem = num_elem;
    strcpy(out_msg.str,str);
    
    return out_msg;
}

position_t initialize_player(client_list* head) {
// Function that initializes the player
// Inputs: pointer to head node
// Outputs: information to be printed (position x,y, character, health)

    srand(time(0)); // initialize random number generators

    position_t init_pos; // initial position
    char c = 64; // '@' in ASCII, 65 = 'A'

    client_list* search_pos = NULL;
    int search_lett = 1; // 1 if found (letter used), 0 if not found (can be used)

    // Generate random position x,y
    init_pos.x = (rand()% (WINDOW_SIZE-2 - 1 + 1)) + 1;
    init_pos.y = (rand()% (WINDOW_SIZE-2 - 1 + 1)) + 1;

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

    // stores the letter and the INITIAL_HEALTH
    init_pos.c = c;
    init_pos.health = INITIAL_HEALTH;
    return init_pos;
}

position_t initialize_bot_prizes(client_list *head, int bot){
// Function that initializes the bots/prizes (similar to the function initialize_player)
// Inputs: pointer to head node, flag bot
// Outputs: information to be printed (position x,y, character, health)

    srand(time(0));
    position_t init_pos;

    // Generate a random position and search in the list if that position is occupied
    init_pos.x = (rand() % (WINDOW_SIZE - 2 - 1 + 1)) + 1;
    init_pos.y = (rand() % (WINDOW_SIZE - 2 - 1 + 1)) + 1;

    while (search_position(head, init_pos.x, init_pos.y) != NULL){ 
        init_pos.x = (rand() % (WINDOW_SIZE - 2 - 1 + 1)) + 1;
        init_pos.y = (rand() % (WINDOW_SIZE - 2 - 1 + 1)) + 1;
    }

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


// Functions for graphical part (windows and draw players, bots and prizes in the field)

WINDOW* generate_window() {
// Function provided by the Professor

    initscr();		    	/* Start curses mode 		*/
	cbreak();				/* Line buffering disabled	*/
    keypad(stdscr, TRUE);   /* We get F1, F2 etc..		*/
	noecho();			    /* Don't echo() while we do getch */

    /* creates a window and draws a border */ 
    WINDOW * my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(my_win, 0 , 0);	
	wrefresh(my_win);
    keypad(my_win, true); 

    /* creates a window and draws a border  */
    message_win = newwin(12, WINDOW_SIZE, WINDOW_SIZE, 0);
    box(message_win, 0 , 0);	
	wrefresh(message_win);

    return(my_win);
}

void draw_player(WINDOW *win, position_t * player, int delete){
// Function provided by the Professor

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

void draw_health(position_t * player, int to_do, int conn_client) {
// Function prints the health of the player in the message window
// Inputs: information to be printed, flag to-do, flag connected client
// Outputs: --

    // to_do : 0 - iniciar msg box health, 1 - editar player health, 2 - eliminar player health
    int aux = 1;
    int c = 65; // 'A' in ASCII
 
    // If it is one client that just connected, print its health
    if(conn_client) { 
        mvwprintw(message_win, 1,2,"%c: %d", player->c, player->health);
        wrefresh(message_win);
    }
    else {
        // Find which player (by its character)
        if (player != NULL) {
            while( c != player->c){
                c++;
                aux++;
            }
        }
        switch (to_do)
        {
        // Initialize message window
        case 0: 
            for(int i=1; i<=MAX_PLAYERS; i++) {
                mvwprintw(message_win, i,2,"-----");
            }
            wrefresh(message_win);
            break;

        // Edit player's health
        case 1:
            mvwprintw(message_win, aux,2,"%c:             ", player->c);
            mvwprintw(message_win, aux,2,"%c: %d", player->c, player->health);
            wrefresh(message_win);
            break;
        
        // Deletes player's health, cleans the message window
        case 2:
            mvwprintw(message_win, aux,2,"            ");
            mvwprintw(message_win, aux,2,"-----");
            wrefresh(message_win);
            break;
        
        default:
            break;
        }

    }
}

void move_client (client_list* client, WINDOW* win, int x, int y){
// Function that moves the player/bots and draws the new position
// Inputs: pointer to the node to be updated, window, position x,y
// Outputs: -- 

    position_t new_play;
    
    // Updates position
    client->x = x;
    client->y = y;

    // We have to pass a struct
    new_play.c = client->c;
    new_play.x = client->x;
    new_play.y = client->y;

    // Draws the new position
    draw_player(win, &new_play, true);
}