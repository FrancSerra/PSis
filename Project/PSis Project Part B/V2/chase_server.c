#include "chase.h"

// Global variables
client_list *head; 
WINDOW *my_win;

void* client_thread(void* arg){

    int sock_fd = *(int*)arg;
    int nbytes, err_update, delete_client_err;
    message_t in_msg, out_msg;


    while(1){
        nbytes = recv(sock_fd, &in_msg, sizeof(in_msg), 0);
        if(nbytes != sizeof(message_t) || in_msg.type != ball_mov){
            break;
        }

        // updates the board
        err_update = update_client(head, in_msg.pid, in_msg.direction, my_win);

        // // If an error occurs, returns an error message to the player
        // if (err_update == -1){
        //     sprintf(msg, " ");
        //     out_msg_ballmov = msg2send_ballmov(error, num_elem, msg);

        // }
        // else{ // If the update is successful

        //     //encodes the field status in an unique string
        //     msg_aux = field2msg(head);
        //     strcpy(msg, msg_aux);

        //     //and sends it to the player for him to update it's own board.
        //     out_msg_ballmov = msg2send_ballmov(field_stat, num_elem, msg);
        //     free(msg_aux);
        // }

        //     sendto(server_sock, &out_msg_ballmov, sizeof(message_ballmov_t), 0, (struct sockaddr *)&client_address, sizeof(client_address));
        
    }

    //deletes the client from the list of clients
    delete_client_err = delete_client(head, in_msg.pid, my_win);
    if (delete_client_err == -1){
        printf("Error disconnecting client.\n");
    }
    else{
        //decreases number of players
        num_elements--;
    }


    close(sock_fd);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int server_sock, client_sock;
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sock == -1){
		perror("socket: ");
		exit(-1);
	}

	struct sockaddr_in server_address, client_address;
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(SOCK_PORT);
	server_address.sin_addr.s_addr = INADDR_ANY;
    socklen_t client_addr_size = sizeof(struct sockaddr_in);


    // Variables declaration and initialization
    // Local variables
    pthread_t thread_id;
    message_t in_msg, out_msg;
    int nbytes, flag_thread;
    position_t init_pos;
    int new_client_err;

    // Global variables
    num_elements = 0;
    head = create_head_client_list(); // Creates the linked list where all board info is stored; players, bots, prizes
    my_win = generate_window();

    // Create bots_thread and prizes_thread

	int err = bind(server_sock, (struct sockaddr *)&server_address, sizeof(server_address));
	if(err == -1) {
		perror("bind");
		exit(-1);
	}

    err = listen(server_sock, 10);
    if(err == -1) {
        perror("listen");
        exit(-1);
    }

    while(1){
        client_sock = accept(server_sock, (struct sockaddr *) &client_address, &client_addr_size);
        if(client_sock < 0){
            printf("Error while accepting connection to server.\n");
        }
        else {
            nbytes = recv(client_sock, &in_msg, sizeof(in_msg), 0);
            if(nbytes == sizeof(message_t) && in_msg.type == conn){

                if (num_elements < (WINDOW_SIZE*WINDOW_SIZE)) {
                    
                    init_pos = initialize_player(head);     //Initializes the player position: assignes an empty board position and unused letter
                    new_client_err = insert_new_client(head, in_msg.pid, init_pos.c, init_pos.x, init_pos.y, INITIAL_HEALTH, client_sock);   // adds the player client to the list
                    
                    if (new_client_err != -1){      //if successfully added to the list

                        //increments the number of players
                        num_elements++; 

                        // Draws the new player in the server board

                        draw_player(my_win, &init_pos, true);
                        draw_health(&init_pos, 1, false);

                        //sends a message to the player containing the assigned, position and character 
                        out_msg = msg2send(ball_info, in_msg.pid, init_pos.c, init_pos.x, init_pos.y, -1, INITIAL_HEALTH);
                        flag_thread = 1;

                    }
                    else{  //case error allocating memory
                        out_msg = msg2send(error, in_msg.pid, UNUSED_CHAR, -1, -1, -1, -1);
                        flag_thread = 0;
                    }
                }
                else{ //case exceed number of elements
                    //sends an error message to the player
                    out_msg = msg2send(error, in_msg.pid, UNUSED_CHAR, -1, -1, -1, -1);
                    flag_thread = 0;
                }
                
                send(client_sock, &out_msg, sizeof(message_t), 0);
                
                if(flag_thread == 1) {
                    pthread_create(&thread_id, NULL, client_thread,(void*)&client_sock);
                }
                else {
                    close(client_sock);
                }
            }
            else{
                perror("Error");
                close(client_sock);
            }
        }

    }
    // 
}