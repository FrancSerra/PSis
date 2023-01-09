#include "chase.h"

int main(int argc, char *argv[]){

    // Client receives the server adress as command line argument
    if (argc != 3){
        printf("Error: Missing server adress or port.\n");
        exit(1);
    }

    // Create the socket 
    int client_sock;
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(client_sock == -1){
        perror("socket: ");
        exit(-1);
    }

    // Store server information in a struct sockaddr_in
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    // server_address.sin_port = htons(SOCK_PORT);
    server_address.sin_port = htons(atoi(argv[2]));

    message_t in_msg, out_msg;
    int nbytes;

    //int err = inet_aton("127.0.0.1",&server_address.sin_addr);
    // inet_pton(AF_INET, argv[1], &server_address.sin_addr)


    // If the provided address is invalid, exit.
    if( inet_aton(argv[1], &server_address.sin_addr) < 1){
        printf("Error: Not a valid IP address: \n");
        exit(-1);
    }

    // generates own ID
    int client_pid = getpid();

    // Connect to Server
    int err = connect(client_sock, (struct sockaddr*) &server_address, sizeof(server_address));
    if(err == -1){
        perror("connect");
        close(client_sock);
        exit(-1);
    }

    out_msg = msg2send(conn, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
    send(client_sock, &out_msg, sizeof(message_t), 0); 

    nbytes = recv(client_sock, &in_msg, sizeof(in_msg), 0);

    if (nbytes != sizeof(message_t)) {
        perror("Error");
        close(client_sock);
        exit(-1);
    }

    switch (in_msg.type){
        case error:
            printf("Error connecting this client.\n");
            close(client_sock);
            exit(-1);
        case ball_info:  // stores the assigned parameters by the server
            player.c = in_msg.c;
            player.x = in_msg.x;
            player.y = in_msg.y;
            player.health = in_msg.health;
            
            printf("type: %d\npid: %d\nc: %d\nx: %d\ny: %d\ndirection: %ld\nhealth: %d\n",in_msg.type,in_msg.pid,in_msg.c,in_msg.x,in_msg.y,in_msg.direction,in_msg.health);
            // field[0] = player;
            // len = 1;
            break; // go to the main loop
        default:
            printf("Error: You have been disconnected.\n");
            close(client_sock);
            exit(-1);
    }
    
    // printf("type: %d\npid: %d\nc: %d\nx: %d\ny: %d\ndirection: %ld\nhealth: %d\n",in_msg.type,in_msg.pid,in_msg.c,in_msg.x,in_msg.y,in_msg.direction,in_msg.health);
          
    while(1){
        
    }
    
    
    close(client_sock);
}