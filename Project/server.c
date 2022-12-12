#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>  
#include <unistd.h>
#include <ncurses.h>

#include "library.h"

#define SERVER_ADDRESS "/tmp/server_socket"

int main()
{
    int server_sock;
    struct sockaddr_un server_address, client_address;
    int client_address_size;
    client_address_size = sizeof(client_address);

    ////////////////////SOCKET CREATION////////////////////
    server_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (server_sock == -1){
		perror("socket: ");
		exit(-1);
	}


    ////////////////////SERVER SOCKET IDENTIFICATION////////////////////
    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, SERVER_ADDRESS);

    unlink(SERVER_ADDRESS);
    int err = bind(server_sock, (struct sockaddr *)&server_address, sizeof(server_address));

    if(err == -1) {
		perror("bind");
		exit(-1);
	}
    
    // //////////////////WINDOW//////////////////
    // initscr();			    /* Start curses mode 		*/
	// cbreak();				/* Line buffering disabled	*/
	// keypad(stdscr, TRUE);	/* We get F1, F2 etc..		*/
	// noecho();		    	/* Don't echo() while we do getch */

    // /* creates a window and draws a border */
    // WINDOW * my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    // box(my_win, 0 , 0);	
	// wrefresh(my_win);


    char ch; 
    int pos_x;
    int pos_y;
    int n_bytes;

    remote_message_t in_msg, out_msg;
    direction_t  direction;


    players_info_t players_info[10];
    int n_players = 0;

    while(1) {
        //receives incoming message
        n_bytes = recvfrom(server_sock, &in_msg, sizeof(remote_message_t), 0, (struct sockaddr *) &client_address, &client_address_size);
   
        //Ignore the message if it is not of the type pre-defined
        if (n_bytes != sizeof(remote_message_t)){
            continue;
        }

        printf("received %d\n", in_msg.msg_type);

        

         // If the message is of the connect type
        if (in_msg.msg_type == 0){
            


            //Checks if there are already 10 players
            if (n_players < 10){
                //Assign the atributes to the new player
                ch = 'f';
                pos_x = WINDOW_SIZE/2;
                pos_y = WINDOW_SIZE/2;
                // Add the new player do the list
                players_info[n_players].ch = ch;
                players_info[n_players].pos_x = pos_x;
                players_info[n_players].pos_y = pos_y;
                n_players++;

                out_msg.msg_type = 0;  //// Se meter ch  apenasfuniona !!!!!!!!
                out_msg.ch = players_info[n_players].ch; //// Se meter ch  apenasfuniona 
          
                sendto(server_sock, &out_msg, sizeof(out_msg), 0, (const struct sockaddr *) &client_address, sizeof(client_address));
                
            }else{
                //send a message saying its full!!!
                out_msg.msg_type = 2;
                sendto(server_sock, &out_msg, sizeof(out_msg), 0, (const struct sockaddr *) &client_address, sizeof(client_address));

            }                                       
        }
    }

}