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

    int client_pid;
    client_pid =  getpid();

    message_t in_msg, out_msg;
    int n_bytes;

    out_msg = msg2send(conn, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
    sendto(client_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr*) &server_address, sizeof(server_address));

    n_bytes = recvfrom(client_sock, &in_msg, sizeof(message_t), 0, (struct sockaddr *) &server_address, &server_address_size);
    if(n_bytes == sizeof(message_t)) {
        if(in_msg.type == error) {
            printf("Error connecting this client.\n");
            exit(-1);
        }
        else if(in_msg.type == ball_info) {
            // while .....
        }
        else {
            out_msg = msg2send(disconn, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
            sendto(client_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr*) &server_address, sizeof(server_address));
            printf("Error: You have been disconnected.\n");
            exit(-1);
        }

    }
    else { // error n_bytes != sizeof(message_t): message not received
        out_msg = msg2send(disconn, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
        sendto(client_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr*) &server_address, sizeof(server_address));
        printf("Error: You have been disconnected.\n");
        exit(-1);
    }
    printf("type: %d\n x: %d\n y:%d\n c: %c\n health:%d\n", in_msg.type, in_msg.x, in_msg.y, in_msg.c, in_msg.health);
}