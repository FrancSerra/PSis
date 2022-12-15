#include "chase.h"

int main()
{
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

    unlink(SERVER_ADDRESS);
    int err = bind(server_sock, (struct sockaddr *)&server_address, sizeof(server_address));

    if(err == -1) {
		perror("bind");
		exit(-1);
	}

    message_t in_msg, out_msg;
    int n_bytes;

    client_list* head = create_head_client_list();
    client_list* player;

    num_players = 0;

    int new_client_err, delete_client_err;
    position_t init_pos;

    WINDOW* my_win = generate_window();
    draw_health(NULL, 0, false);

    while(1)
    {
        n_bytes = recvfrom(server_sock, &in_msg, sizeof(message_t), 0, (struct sockaddr *) &client_address, &client_address_size);
		if(n_bytes == sizeof(message_t)) {
            switch(in_msg.type) {
                case conn:
                    if (num_players < MAX_PLAYERS){
                        num_players ++;
                        init_pos = initialize_player(head);
                        new_client_err = insert_new_client(head, in_msg.pid, init_pos.c, init_pos.x, init_pos.y, INITIAL_HEALTH);
                        if (new_client_err != -1) { // caso seja adicionado à lista com sucesso
                            draw_player(my_win, &init_pos, true);
                            draw_health(&init_pos, 1, false);
                            out_msg = msg2send(ball_info, in_msg.pid, init_pos.c, init_pos.x, init_pos.y, -1, INITIAL_HEALTH);
                        }
                        else { // caso haja erro de alocacao de memoria
                            out_msg = msg2send(error, in_msg.pid, UNUSED_CHAR, -1, -1, -1, -1);
                        }
                        
                    }
                    else { // caso já haja 10 players
                        out_msg = msg2send(error, in_msg.pid, UNUSED_CHAR, -1, -1, -1, -1);
                    }
                    sendto(server_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr*) &client_address, sizeof(client_address));
                    break;
                case ball_mov:
                    player = update_client(head, in_msg.pid, in_msg.direction, my_win);
                    if (player == NULL) {
                        out_msg = msg2send(error, in_msg.pid, UNUSED_CHAR, -1, -1, -1, -1);
                    }
                    else { 
                        // field_status	
                    } 
                    break; 
                case disconn:	
                    delete_client_err = delete_client(head, in_msg.pid, my_win);
                    if (delete_client_err == -1) {
                        printf("This client was not yet connected.\n");
                    }
                    else {
                        num_players --;
                    }
                    break;
                    
                default: 
                    break;
            }
        }
        else {
            perror("Fails to receive message.\n");

        }
    }
    
}