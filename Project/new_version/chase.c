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

char ascii2char() {
    int a = 65;
    char c = a+num_players-1;

    return c;
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

init_pos_t init_position (client_list* head) {
    srand(time(0));
    init_pos_t init_pos;
    int aux_search = 1; // aux_search: = 1 if found (position held) = 0 if not found

    while (aux_search == 1) {
        init_pos.x = (rand()% (WINDOW_SIZE-2 - 1 + 1)) + 1;
        init_pos.y = (rand()% (WINDOW_SIZE-2 - 1 + 1)) + 1;

        aux_search = search_position(head, init_pos.x, init_pos.y);
    }
    return init_pos;
}