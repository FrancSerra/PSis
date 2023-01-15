#include"fcn_client.h"


void mng_field_status(message_fieldstat_t msg){
// Function that manages the field after receiving a field_status message
// Inputs: message of type message_fieldstat_t received

    // Flag indicates what type of update it is
    switch (msg.flag){

    // In case just a new player or a new prize appeared 
    // Receives in the old_position and in the new_position the same element (prize or player)
    // Only checks the new_position and draws it     
    case 0:
        draw_player(my_win, &msg.new_pos, true);

        // Only draws the health in the message window if it is a player and not a prize
        if (msg.new_pos.health != -1){
            draw_health(&msg.new_pos, 0);
        }
        break;

    // In case of a normal movement
    // Player or bot moved (or stayed in the same position in case it tried to move to the border)
    //      Receives the old and the new position
    // Player eats a prize
    //      Receives the player old and new positions and checks if its health changed
    case 1:

        // Only deletes it from the previous position and draws it in the new one in case it changed
        if(msg.old_pos.x != msg.new_pos.x || msg.old_pos.y != msg.new_pos.y){
            draw_player(my_win, &msg.old_pos, false);
            draw_player(my_win, &msg.new_pos, true);

            // Checks if the health has changed an updates it if so (only in case it is a player and not a bot)
            if(msg.old_pos.health != msg.new_pos.health && msg.old_pos.c != BOT_CHAR){
                draw_health(&msg.new_pos, 0);
            }
        }
        break;

    // In case two players collid or a bot rams into a player (positions do not change)
    // Or in case a player sends a continue_message the other players update its health
    case 2:
        
        // Draws the new health of the player who was hit
        draw_health(&msg.new_pos, 0);

        // If the previous player was hit by another player updates this player' health
        if(msg.old_pos.c != BOT_CHAR && msg.old_pos.c != msg.new_pos.c){
            draw_health(&msg.old_pos, 0);
        }
        break;

    // In case a player disconnects other players delete its old position and health
    case 3:
        draw_player(my_win, &msg.old_pos, false);
        draw_health(&msg.old_pos, 1);
        break;
    
    default:
        break;
    }

}

// Handler functions

void sig_handler(int signum){
// Function thats is called when the timer runs out 

    // Prints the information that the key was not pressed
    mvwprintw(error_win, 1,1,"Key not pressed.\n Game over!\n");
    wrefresh(error_win);

    // Closes the client socket and terminates the program
    close(client_sock);
    exit(-1);
}

void interupt_handler_client(int s){
// Function that is called when a Ctrl+C is detected

    // Prints the information, closes the socket and terminates the program
    mvwprintw(error_win, 1,1,"Caught Ctrl+C!\n\n\n");
    wrefresh(error_win);
    close(client_sock);
    exit(-1); 
}
