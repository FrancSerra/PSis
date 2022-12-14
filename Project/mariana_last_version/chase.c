#include "chase.h"

// Lists

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

int delete_client(client_list* head, int pid, WINDOW* win){

    // Store head node
    client_list *temp = head->next, *prev;

    position_t delete_pos;
 
    // If head node itself holds the key to be deleted
    if (temp != NULL && temp->pid == pid) {
        head->next = temp->next; // Changed head
        delete_pos.c = temp->c;
        delete_pos.x = temp->x;
        delete_pos.y = temp->y;
        draw_player(win, &delete_pos, false);
        draw_health(&delete_pos, 2, false);

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
    
    delete_pos.c = temp->c;
    delete_pos.x = temp->x;
    delete_pos.y = temp->y;
    draw_player(win, &delete_pos, false);
    draw_health(&delete_pos, 2, false);

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

client_list* search_position(client_list* head, int x, int y){

    client_list* temp;
    for (temp = head->next; temp != NULL; temp = temp->next)
    {
        if (temp->x == x && temp->y == y){
            return temp; // returns 1 if found (position held)
        }
    }

    return temp; //returns NULL if not found
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

client_list* search_client(client_list* head, int pid){

    client_list* temp;
    for (temp = head->next; temp != NULL; temp = temp->next)
    {
        if (temp->pid == pid){
            return temp; // returns 1 if found (letter already assigned)
        }
    }

    return temp; // returns NULL if not found
}

client_list* update_client(client_list* head, int pid, int direction, WINDOW* win){
    
    client_list* player;
    client_list* other_player;
    position_t new_play, old_play;

    player = search_client(head, pid);
    int x, y; // aux
    x = player->x;
    y = player->y;

    old_play.c = player->c;
    old_play.x = player->x;
    old_play.y = player->y;

    // If client was not present in linked list
    if (player == NULL)
        return player;
    
    if (direction == KEY_UP){
        if (y  != 1){
            y --;
        }
    }

    if (direction == KEY_DOWN){
        if (y  != WINDOW_SIZE-2){
            y ++;
        }
    }

    if (direction == KEY_LEFT){
        if (x  != 1){
            x --;
        }
    }

    if (direction == KEY_RIGHT)
        if (x  != WINDOW_SIZE-2){
            x ++;
    }
    
    other_player = search_position(head, x, y);

    if (other_player == NULL) {
        // search bot
        // search prize tudo dentro de sucessivos ifs - so muda a pos no ultimo if 
                    player->x=x;
                    player->y=y;

                    draw_player(win, &old_play, false);
                    new_play.c = player->c;
                    new_play.x = player->x;
                    new_play.y = player->y;
                    draw_player(win, &new_play, true);
                    
    }
    else {
        player->health ++;
        other_player->health --;

        new_play.c = player->c;
        new_play.health = player->health;
        draw_health(&new_play, 1, false);
        new_play.c = other_player->c;
        new_play.health = other_player->health;
        draw_health(&new_play, 1, false);
    }

    return player;
}

// Comms

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

position_t initialize_player(client_list* head) {
    srand(time(0));
    position_t init_pos;
    char c = 64; // '@' in ASCII, 65 = 'A'
    client_list* search_pos = NULL;
    int search_lett = 1; // 1 if found (letter used) = 0 if not found

    init_pos.x = (rand()% (WINDOW_SIZE-2 - 1 + 1)) + 1;
    init_pos.y = (rand()% (WINDOW_SIZE-2 - 1 + 1)) + 1;
    search_pos = search_position(head, init_pos.x, init_pos.y);
    
    while (search_pos != NULL || search_lett == 1){
        if (search_pos != NULL) {
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
    init_pos.health = INITIAL_HEALTH;
    return init_pos;
}

// Graphics
WINDOW* generate_window() {
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

void draw_health(position_t * player, int to_do, int conn_client) {
    // to_do : 0 - iniciar msg box health, 1 - editar player health, 2 - eliminar player health
    int aux = 1;
    int c = 65;

    if(conn_client) { // client window: beggining
        mvwprintw(message_win, 1,2,"%c: %d", player->c, player->health);
        wrefresh(message_win);
    }
    else {
        if (player != NULL) {
            while( c != player->c){
                c++;
                aux++;
            }
        }
        switch (to_do)
        {
        case 0:
            for(int i=1; i<=MAX_PLAYERS; i++) {
                mvwprintw(message_win, i,2,"-----");
            }
            wrefresh(message_win);
            break;
         case 1:
            mvwprintw(message_win, aux,2,"%c:   ", player->c);
            mvwprintw(message_win, aux,2,"%c: %d", player->c, player->health);
            wrefresh(message_win);
            break;
         case 2:
            mvwprintw(message_win, aux,2,"-----");
            wrefresh(message_win);
            break;
        
        default:
            break;
        }

    }
}