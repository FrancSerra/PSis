#include "chase.h"

// Create mutex and rwlocks
static pthread_mutex_t mtx_draw = PTHREAD_MUTEX_INITIALIZER; // mutex to access draw_player and draw_health


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

void reset_windows(WINDOW* win){
    werase(win);
    wrefresh (win);

    werase(message_win);
    wrefresh (message_win);

    box(win, 0 , 0);	
	wrefresh(win);
    keypad(win, true);

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



