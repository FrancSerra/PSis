#include "remote-char.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#include<netinet/in.h>
#include <arpa/inet.h>


#include <ncurses.h>


#define WINDOW_SIZE 15



int main()
{	
    
       

     char linha[100];
   	printf("What is the network address of the recipient? ");
	fgets(linha, 100, stdin);
	linha[strlen(linha)-1] = '\0';


    

	initscr();		    	
	cbreak();				
    keypad(stdscr, TRUE);   
	noecho();			    

    /* creates a window and draws a border */
    WINDOW * my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(my_win, 0 , 0);	
	wrefresh(my_win);

    int ch;
    int pos_x;
    int pos_y;
    int n_bytes;


    remote_char_t m;
    
    while (1){

        if(m.msg_type == 6){
                pos_x = m.x;
                pos_y = m.y;
                ch = m.ch;
                wmove(my_win, pos_x, pos_y);
                waddch(my_win,ch| A_BOLD);
                wrefresh(my_win);			
        }
    }
  	endwin();			/* End curses mode		  */

	return 0;
}