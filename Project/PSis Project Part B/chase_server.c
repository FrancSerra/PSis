#include "chase.h"

//more includes line 34-38 of chase.h

int main(){

    //////////////  Create the socket //////////////
    int server_sock;
    server_sock = socket(AF_INET, SOCK_STREAM, 0);

    if (server_sock == -1){
        perror("socket: ");
        exit(-1);
    }

    //////////////  Socket identification //////////////
    struct sockaddr_in server_address, client_address;

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SOCK_PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;  //If this program can receive messages sent to any address he can use the INADDR_ANY constant

    //bind to allow receiving messages
    int err = bind(server_sock, (struct sockaddr *)&server_address, sizeof(server_address));

    if (err == -1){
        perror("bind");
        exit(-1);
    }

///////////////////////////////////////////////////////////////////////////////////////








}