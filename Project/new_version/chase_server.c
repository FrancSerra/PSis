#include "chase.h"
#include "lists.h"

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
    num_players = 20;

    while(1)
    {
        n_bytes = recvfrom(server_sock, &in_msg, sizeof(message_t), 0, (struct sockaddr *) &client_address, &client_address_size);
		if(n_bytes == sizeof(message_t) && check_message(in_msg)== 0) {
            switch(in_msg.type) {
                case conn:
                    if (num_players < MAX_PLAYERS){
                        num_players ++;

                        out_msg.x = WINDOW_SIZE/2;
                        out_msg.y = WINDOW_SIZE/2;
                        out_msg.c = ascii2char();
                        out_msg.health = 10;

                        int new_client_err = insert_new_client(head, in_msg.pid, out_msg.c, out_msg.x, out_msg.y, out_msg.health);
                        if (new_client_err == 1) {
                            out_msg.type = ball_info;
                        }
                        else {
                            out_msg.type = error;
                        }
                        
                    }
                    else {
                        out_msg.type = error;
                    }
                    sendto(server_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr*) &client_address, sizeof(client_address));
   
                //case ball_mov:
                case disconn:
                    int delete_err = delete_client(head, in_msg.pid);
                    if (delete_err == -1) {
                        printf("This client was not yet connected.\n");
                    }
                    else {
                        num_players --;
                    }
                    
                default: 
                    break;
            }
        }
        else {
            perror("Fails to receive message.\n");
        }
    }
    
}