#include "chase.h"


//more includes line 34-38 of chase.h

int main(int argc, char *argv[]){

    // Client receives the server adress as command line argument

    if (argc != 2){
        printf("Error: Missing server adress.\n");
        exit(1);
    }

    //////////////  Create the socket //////////////
    int client_sock;
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock == -1){
        perror("socket: ");
        exit(-1);
    }

    //////////////  Socket identification //////////////
    struct sockaddr_in server_address;
 
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SOCK_PORT);
    

    if( inet_pton(AF_INET, argv[1], (struct sockaddr *) &server_address.sin_addr) < 1){
		printf("no valid address: \n");
		exit(-1); 
	}

///////////////////////////////////////////////////////////////////////////////////////











}