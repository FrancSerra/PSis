#include "chase.h"

int main()
{

    //////////////  Create the socket //////////////
    int server_sock;
    struct sockaddr_un server_address, client_address;
    int client_address_size = sizeof(client_address);

    server_sock = socket(AF_UNIX, SOCK_DGRAM, 0);

    if (server_sock == -1){
        perror("socket: ");
        exit(-1);
    }

    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, SERVER_ADDRESS);

    //bind to allow receiving messages
    unlink(SERVER_ADDRESS);
    int err = bind(server_sock, (struct sockaddr *)&server_address, sizeof(server_address));

    if (err == -1){
        perror("bind");
        exit(-1);
    }
    /////////////////////////////////////////////
    
    // Creates the linked list where all board info is stored; players, bots, prizes
    client_list *head = create_head_client_list();

    // VARIABLES declaration and initialization

    // GLOBALS
    num_players = 0;
    num_bots = 0;
    num_prizes = 0;
    
    //LOCALS
    message_t in_msg, out_msg;
    message_ballmov_t out_msg_ballmov;
    position_t init_pos;
    int n_bytes;
    int new_client_err, delete_client_err, err_update;
    int aux_if = 0;
    int n_bots = 0;
    int n_prizes = 0;
    int flag = 0;
    int flag_bot_con = 0;
    int flag_prizes_con = 0;
    int client_pid;
    int num_elem;

    long int dirs;

    char msg[BUFFER_SIZE];
    char *msg_aux;
    

    WINDOW *my_win = generate_window();
    draw_health(NULL, 0, false);



    //////////////  MAIN SERVER LOOP //////////////
    while (1){
        // Receives a mesage
        n_bytes = recvfrom(server_sock, &in_msg, sizeof(message_t), 0, (struct sockaddr *)&client_address, (socklen_t *)&client_address_size);

        // Confirms if the message is the one expected

        if (n_bytes == sizeof(message_t)){  
            
            // Obtains client ID
            client_pid = atoi((client_address.sun_path + strlen("/temp/client") - 1));

            switch (in_msg.type){
            
            
            case conn:  //  CONN messages are only sent by players
                
                if (num_players < MAX_PLAYERS){ // Checks if the maximum number of players was achieved
                    
                    init_pos = initialize_player(head);     //Initializes the player position: assignes an empty board position and unused letter
                    new_client_err = insert_new_client(head, client_pid, init_pos.c, init_pos.x, init_pos.y, INITIAL_HEALTH);   // adds the player client to the list
                    
                    if (new_client_err != -1){      // caso seja adicionado à lista com sucesso

                        //increments the number of players
                        num_players++; 

                        // Draws the new player in the server board
                        draw_player(my_win, &init_pos, true);
                        draw_health(&init_pos, 1, false);

                        //sends a message to the player containing the assigned, position and character 
                        out_msg = msg2send(ball_info, client_pid, init_pos.c, init_pos.x, init_pos.y, -1, INITIAL_HEALTH);

                    }else{                          // caso haja erro de alocacao de memoria
                        out_msg = msg2send(error, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
                    }
                }else{ // caso já haja 10 players
                    //sends an error message to the player
                    out_msg = msg2send(error, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
                }

                sendto(server_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr *)&client_address, sizeof(client_address));
                break;

            case ball_mov:  //  Ball movement messages are only sent by players

                // updates the board
                err_update = update_client(head, client_pid, in_msg.direction, my_win);
                num_elem = num_bots + num_players + num_prizes;

                // If an error occurs, returns an error message to the player
                if (err_update == -1){
                    sprintf(msg, " ");
                    out_msg_ballmov = msg2send_ballmov(error, num_elem, msg);

                }else{ // If the update is successful

                    //encodes the field status in an unique string
                    msg_aux = field2msg(head);
                    strcpy(msg, msg_aux);

                    //and sends it to the player for him to update it's own board.
                    out_msg_ballmov = msg2send_ballmov(field_stat, num_elem, msg);
                    free(msg_aux);
                }

                sendto(server_sock, &out_msg_ballmov, sizeof(message_ballmov_t), 0, (struct sockaddr *)&client_address, sizeof(client_address));
                break;

            case bot_conn: //  Bot-connecting messages are only received when a bot client wants is launched

                if (flag_bot_con == 0){ // FLAG = 0 --> No BOT client connected
                                        // FLAG = 1 --> Already a BOT client connected

                    flag_bot_con = 1; // Now there is a BOT client connected
                    n_bots = in_msg.health; // HEALTH parameter is the carrier for n_bots info. It's the way it was defined
                    
                    if (num_bots < MAX_BOTS){
                        
                        // num_bots --> number of bots already in the game
                        // n_bots --> number of bots the BOT client wants to include
                        int sum = num_bots + n_bots;  
                        
                        // if there are already BOTs in the game, the number of BOTs the BOT client can add
                        // is only the ramaining number to reach the MAX value
                        if (sum > MAX_BOTS){
                            n_bots = MAX_BOTS - num_bots;
                        }

                        // Adds the BOTs to the game                      
                        for (int i = 0; i < n_bots; i++){

                            // //Initializes BOT(s). Assigns it's representation (´*') and an empty board position.
                            init_pos = initialize_bot_prizes(head, true);

                            // Adds the BOT(s) into the list of clients ( BOT is also a client )
                            new_client_err = insert_new_client(head, client_pid, init_pos.c, init_pos.x, init_pos.y, -1); // health doesn't have a meaning for the bot
                            if (new_client_err != -1){
                                draw_player(my_win, &init_pos, true);
                                num_bots++;

                            }else{
                                aux_if = 1;
                                break;
                            }
                        }

                        if (aux_if){ // if the insertion int he list went wrong
                            // sends an error message
                            out_msg = msg2send(error, client_pid, UNUSED_CHAR, -1, -1, -1, -1);

                        }else{
                            // Tells BOT client that he can start sending messages
                            out_msg = msg2send(bot_conn, client_pid, UNUSED_CHAR, -1, -1, -1, n_bots);
                        }

                    }else{
                        // If the number of BOTs surpases 10, sends an error message
                        out_msg = msg2send(error, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
                    }
                    
                }else{  // If there is already a BOT client connected
                    out_msg = msg2send(error, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
                }

                sendto(server_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr *)&client_address, sizeof(client_address));
                break;

            case bot_mov: // BOT-moves messages are only snet by BOT clients

                // Receives the directions of each running bot encoded by each ordered digit of the integer received in direction parameter of in_msg structure
                dirs = in_msg.direction;

                client_list *aux;
                aux = head; // pointer to the list of clients

                // processes each direction at a time, by reading the last digit 
                while (dirs > 0){

                    int mod = dirs % 10; // gets the last digit from number

                    //mod is now the direction, so it updates the bot position
                    aux = update_bot(head, aux, mod, my_win);

                    dirs = dirs / 10; // removes the last digit so that the next dir info is available for the next iteration
                }
                break;

            case prizes_conn:

                // Case 1: When the game starts, 5 prizes are placed randomly in the field.
                //after, starting it puts 1 prize every second

                //Case 2: flag encodes if it is the start case 1 or case 2
                flag = in_msg.health; // Case 1: flag = 0; Case 2: flag = 1       
                if (flag == 0){
                    n_prizes = 5;

                }else if (flag == 1){
                    n_prizes = 1;
                    flag_prizes_con = 0;

                }else{  // if none of the above cases : error message
                    out_msg = msg2send(error, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
                    sendto(server_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr *)&client_address, sizeof(client_address));
                    break;
                }


                if (flag_prizes_con == 0){  // FLAG = 0 --> Case 1: No Prizes client connected 
                                            // FLAG = 1 --> Case 2: Already a Prizes client connected
                
                    if (num_prizes < MAX_PRIZES){

                        flag_prizes_con = 1; // Now there is a Prizes client connected
                        // num_prizes --> number of prizes already in the game
                        // n_prizes --> number of prizes the Prizes client wants to include
                        int sum = num_prizes + n_prizes;

                        // if there are already Prizes in the game, the number of Prizes the Prizes client can add
                        // is only the ramaining number to reach the MAX value

                        if (sum > MAX_PRIZES){
                            n_prizes = MAX_PRIZES - num_prizes;
                        }

                        for (int i = 0; i < n_prizes; i++){

                            // Initializes prize(s) Assigns it's representation (´1, 2, etc') and an empty board position. 
                            init_pos = initialize_bot_prizes(head, false);

                            // Adds the Prizes(s) into the list of clients ( Prizes  is also a client ) 
                            new_client_err = insert_new_client(head, client_pid, init_pos.c, init_pos.x, init_pos.y, -1); // health doesn't have a meaning for the prizes
                            
                            if (new_client_err != -1){       
                                // draws in the board            
                                draw_player(my_win, &init_pos, true);
                                // increments the number of prizes
                                num_prizes++;

                            }else{
                                aux_if = 1;
                                break;
                            }
                        }

                        if (aux_if){
                            out_msg = msg2send(error, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
                        }else{
                            out_msg = msg2send(prizes_conn, client_pid, UNUSED_CHAR, -1, -1, -1, n_prizes);
                        }

                    }else{  //In case there are already 10 prizes
                        n_prizes = 0;
                        out_msg = msg2send(prizes_conn, client_pid, UNUSED_CHAR, -1, -1, -1, n_prizes);
                    }
                    
                }else{
                    out_msg = msg2send(error, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
                }


                sendto(server_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr *)&client_address, sizeof(client_address));
                break;

            case disconn:  // IF a client wants to disconnect. Only players send 

                //deletes the client from the list of clients
                delete_client_err = delete_client(head, client_pid, my_win);

                if (delete_client_err == -1){
                    printf("Error disconnecting client.\n");

                }else{
                    //decreases number of players
                    num_players--;
                }
                break;

            default:
                break;
            }


        }else{ // if none of the types of messages was received: ERROR

            printf("\033[41B");
            printf("\033[6D");
            printf("Fails to receive message.\n");
            printf("\033[1B");
            printf("\033[10D");
        }
    }
}