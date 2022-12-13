#include "chase.h"

client_list* create_head_client_list(){
    
    client_list* head = NULL;
    head = (client_list*) malloc(sizeof(client_list));

    if(head == NULL){ //verificar o sucesso da alocação
        printf("Erro de alocação de memória.\n");
        exit(-1);
    }

    head->next=NULL;

    return head;
}

int insert_new_client(client_list* head, int pid, char c, int x, int y, int health){

    client_list* new_block = NULL;
    
    new_block = (client_list *) malloc(sizeof(client_list));
    
    if(new_block == NULL){ //verificar o sucesso da alocação
        printf("Erro de alocação de memória.\n");
        return -1;
    }

    client_list* last = head->next; //last aponta para o começo da lista

    new_block->pid = pid;
    new_block->c = c;
    new_block->x = x;
    new_block->y = y;
    new_block->health = health;

    new_block->next = NULL; //new_block vai ficar no final da lista, logo aponta para nulo
  
    if (head->next == NULL)      //Se a lista tiver vazia, a new_block fica no topo
    {
       head->next = new_block;
       return 1;
    }  

    while (last->next != NULL){ //Percorre a lista até ao fim     
        last = last->next;        
    }

    last->next = new_block; //Insere o new_block no final da lista

    return 1;
}

int delete_client(client_list* head, int pid){

    // Store head node
    client_list *temp = head->next, *prev;
 
    // If head node itself holds the key to be deleted
    if (temp != NULL && temp->pid == pid) {
        head->next = temp->next; // Changed head
        free(temp); // free old head
        return 1;   //returns 1 if key was present and deleted
    }
 
    // Search for the key to be deleted, keep track of the
    // previous node as we need to change 'prev->next'
    while (temp != NULL && temp->pid != pid) {
        prev = temp;
        temp = temp->next;
    }
 
    // If key was not present in linked list
    if (temp == NULL)
        return -1; //returns 1 if key not present
 
    // Unlink the node from linked list
    prev->next = temp->next;
 
    free(temp); // Free memory
    return 1;   //returns 1 if key was present and deleted
}

void print_client_list(client_list* node){

    node=node->next;
    while (node != NULL)
    {
       printf("PID: %d\n Letter: %c\n Pos_x: %d\n Pos_y: %d\n Health: %d\n", node->pid, node->c, node->x ,node->y, node->health);
       node = node->next;
    }
}

int search_position(client_list* head, int x, int y){

    for (client_list* temp = head->next; temp != NULL; temp = temp->next)
    {
        if (temp->x == x && temp->y == y){
            return 1; // returns 1 if found (position held)
        }
    }

    return 0; //returns 0 if not found
}

int search_letter(client_list* head, char c){

    for (client_list* temp = head->next; temp != NULL; temp = temp->next)
    {
        if (temp->c == c){
            return 1; // returns 1 if found (letter already assigned)
        }
    }

    return 0; //returns 0 if not found
}

int check_message(message_t msg){
    switch(msg.type){
        case conn:
            break;
        case ball_info:
            break;
        case ball_mov:
            if (msg.x < 1 || msg.x > WINDOW_SIZE-2 || msg.y < 1 || msg.y > WINDOW_SIZE-2)
                return -1;
            break;
        case field_stat:
            break;
        case health0:
            break;
        case disconn:
            break;
        default:
            return -1;
    }
    return 0;
}

message_t msg2send(msg_type type, int pid, char c, int x, int y, int direction, int health) {
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

position_t initialize_player() {
    srand(time(0));
    position_t init_pos;
    char c = 64; // '@' in ASCII, 65 = 'A'
    int search_pos = 1; // 1 if found (position held) 0 if not found
    int search_lett = 1; // 1 if found (letter used) = 0 if not found
    
    while (search_pos == 1 || search_lett == 1){
        if (search_pos == 1) {
            init_pos.x = (rand()% (WINDOW_SIZE-2 - 1 + 1)) + 1;
            init_pos.y = (rand()% (WINDOW_SIZE-2 - 1 + 1)) + 1;

            search_pos = search_position(head, init_pos.x, init_pos.y);
        }
         if (search_lett == 1) {
            c++;
            search_lett = search_letter(head, c); 
        }
    }
    init_pos.c = c;
    return init_pos;
}

// Functions provided by the Prof

void new_player (position_t * player, char c){
    player->x = WINDOW_SIZE/2;
    player->y = WINDOW_SIZE/2;
    player->c = c;
}

void draw_player(WINDOW *win, position_t * player, int delete){
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

void moove_player (position_t * player, int direction){
    if (direction == KEY_UP){
        if (player->y  != 1){
            player->y --;
        }
    }

    if (direction == KEY_DOWN){
        if (player->y  != WINDOW_SIZE-2){
            player->y ++;
        }
    }

    if (direction == KEY_LEFT){
        if (player->x  != 1){
            player->x --;
        }
    }

    if (direction == KEY_RIGHT)
        if (player->x  != WINDOW_SIZE-2){
            player->x ++;
    }
}
