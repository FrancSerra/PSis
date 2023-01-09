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

int insert_new_client(client_list* head, int pid, char c, int x, int y, int health, int socket_id){
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

// int delete_client(client_list* head, int pid, WINDOW* win){
// // Function that deletes a player from the list of clients and the screen
// // Inputs: pointer to the head of the list, player PID, window
// // Outputs: 1 if successfully deleted, -1 if it was not in the list

//     client_list *temp = head->next, *prev; // stores head of the list

//     position_t delete_pos; // information to be deleted
 
//     // If head node itself holds the key to be deleted
//     if (temp != NULL && temp->pid == pid) {
//         head->next = temp->next; // Changed head
//         delete_pos.c = temp->c;
//         delete_pos.x = temp->x;
//         delete_pos.y = temp->y;
//         close(temp->socket_id);

//         // delete player and its health from the window

//         // draw_player(win, &delete_pos, false);
//         // draw_health(&delete_pos, 2, false);

//         free(temp); // free old head
//         return 1;   
//     }
 
//     // Search for the key to be deleted, keep track of the previous node as we need to change 'prev->next'
//     while (temp != NULL && temp->pid != pid) {
//         prev = temp;
//         temp = temp->next;
//     }
 
//     // If key was not present in linked list
//     if (temp == NULL)
//         return -1;
 
//     // Unlink the node from linked list
//     prev->next = temp->next;
    
//     delete_pos.c = temp->c;
//     delete_pos.x = temp->x;
//     delete_pos.y = temp->y;
//     close(temp->socket_id);

//     // delete player and its health from the window 

//     // draw_player(win, &delete_pos, false);
//     // draw_health(&delete_pos, 2, false);

//     free(temp); // Free memory
//     return 1;   //returns 1 if key was present and deleted
// }

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