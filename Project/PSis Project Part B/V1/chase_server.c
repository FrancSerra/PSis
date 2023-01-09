#include "chase.h"

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
    message_t in_msg, out_msg;
    int nbytes, flag_thread;
    position_t init_pos;
    int new_client_err;

    // Global variables
    num_elements = 0;


    // Creates the linked list where all board info is stored; players, bots, prizes
    client_list *head = create_head_client_list();


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
                // printf("type: %d\npid: %d\nc: %d\nx: %d\ny: %d\ndirection: %ld\nhealth: %d\n",in_msg.type,in_msg.pid,in_msg.c,in_msg.x,in_msg.y,in_msg.direction,in_msg.health);

                if (num_elements < (WINDOW_SIZE*WINDOW_SIZE)) {
                    
                    init_pos = initialize_player(head);     //Initializes the player position: assignes an empty board position and unused letter
                    new_client_err = insert_new_client(head, in_msg.pid, init_pos.c, init_pos.x, init_pos.y, INITIAL_HEALTH, client_sock);   // adds the player client to the list
                    
                    if (new_client_err != -1){      // caso seja adicionado à lista com sucesso

                        //increments the number of players
                        num_elements++; 

                        // Draws the new player in the server board

                        // draw_player(my_win, &init_pos, true);
                        // draw_health(&init_pos, 1, false);

                        //sends a message to the player containing the assigned, position and character 
                        out_msg = msg2send(ball_info, in_msg.pid, init_pos.c, init_pos.x, init_pos.y, -1, INITIAL_HEALTH);
                        flag_thread = 1;

                    }
                    else{                          // caso haja erro de alocacao de memoria
                        out_msg = msg2send(error, in_msg.pid, UNUSED_CHAR, -1, -1, -1, -1);
                        flag_thread = 0;
                    }
                }
                else{ // case exceed number of elements
                    //sends an error message to the player
                    out_msg = msg2send(error, in_msg.pid, UNUSED_CHAR, -1, -1, -1, -1);
                    flag_thread = 0;
                }
                
                send(client_sock, &out_msg, sizeof(message_t), 0);
                printf("type: %d\npid: %d\nc: %d\nx: %d\ny: %d\ndirection: %ld\nhealth: %d\n",out_msg.type,out_msg.pid,out_msg.c,out_msg.x,out_msg.y,out_msg.direction,out_msg.health);
            

                if(flag_thread == 1) {
                    printf("Criar thread\n");
                    // pthread_create(&thread_id, NULL, client_thread,(void*)&client_socket_local);
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
// out_msg = msg2send(confirm, -1, UNUSED_CHAR, -1, -1, -1, -1);
// send(client_sock, &out_msg, sizeof(message_t), 0);