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

#include "chase.h"

int main(int argc, char *argv[])
{
    if (argc != 2){
        printf("Error: Missing recipient IP.\n");
        exit(1);
    }

    int client_sock;
    struct sockaddr_un server_address, client_address;
    int server_address_size = sizeof(client_address);

    client_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (client_sock == -1){
		perror("socket: ");
		exit(-1);
	}

    client_address.sun_family = AF_UNIX;
    sprintf(client_address.sun_path, "/tmp/client%d", getpid());

    unlink(client_address.sun_path);
    int err = bind(client_sock, (const struct sockaddr *) &client_address, sizeof(client_address));
    if(err == -1) {
        perror("bind");
        exit(-1);
    }

    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, argv[1]);

    message_t in_msg, out_msg;

    out_msg.pid = getpid();
    out_msg.type = conn;
    sendto(client_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr*) &server_address, sizeof(server_address));
    recvfrom(client_sock, &in_msg, sizeof(message_t), 0, (struct sockaddr *) &server_address, &server_address_size);
    
    printf("type: %d\n x: %d\n y:%d\n c: %c\n health:%d\n", in_msg.type, in_msg.x, in_msg.y, in_msg.c, in_msg.health);
}