#include "chase.h"

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        printf("Error: Missing one argument.\n");
        exit(1);
    }

    // Number of bots
    int n_bots = atoi(argv[2]);
    int is_zero = strcmp(argv[2], "0");

    if (n_bots < 0 || n_bots > MAX_BOTS)
    {
        printf("Error: not valid number of bots.\nChoose a number between 0 and %d.\n", MAX_BOTS);
        exit(1);
    }

    if (n_bots == 0)
    {
        if (is_zero != 0)
        {
            printf("Error: not valid number of bots.\nChoose a number between 0 and %d.\n", MAX_BOTS);
            exit(1);
        }
        else
        {
            printf("Zero bots added. Task completed.\n");
            exit(1);
        }
    }

    int bot_client_sock;
    struct sockaddr_un server_address, bot_client_address;
    int server_address_size = sizeof(server_address);

    // Initialize socket
    bot_client_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (bot_client_sock == -1)
    {
        perror("socket: ");
        exit(-1);
    }

    // Get bot client PID 
    int bot_client_pid;
    bot_client_pid = getpid();

    bot_client_address.sun_family = AF_UNIX;
    sprintf(bot_client_address.sun_path, "/tmp/client%d", bot_client_pid);

    unlink(bot_client_address.sun_path);
    // Check if bind error
    int err = bind(bot_client_sock, (const struct sockaddr *)&bot_client_address, sizeof(bot_client_address));
    if (err == -1)
    {
        perror("bind");
        exit(-1);
    }

    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, argv[1]);

    message_t out_msg, in_msg;

    // Number of bots is stored in health parameter
    out_msg = msg2send(bot_conn, bot_client_pid, UNUSED_CHAR, -1, -1, -1, n_bots);

    // Send connection message to the server
    sendto(bot_client_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr *)&server_address, sizeof(server_address));
    printf("Connect message sent\n");

    int n_bytes;

    // Receive message from the server
    n_bytes = recvfrom(bot_client_sock, &in_msg, sizeof(message_t), 0, (struct sockaddr *)&server_address, (socklen_t *)&server_address_size);
    
    // Fail in receiving message
    if (n_bytes != sizeof(message_t)){
        printf("Failed communication.\nYou have not been connected.\n");
        exit(-1);
    }

    if (in_msg.type != bot_conn){
        printf("Failed connecting.\n");
        exit(-1);
    }
    else{
        // Successfully connected
        printf("Added %d out of %d bots.\n", in_msg.health, n_bots);
        n_bots = in_msg.health;
        printf("Bots running...\n");

        int rand_number;
        char s1[2];
        char s2[MAX_BOTS + 1];
        srand(time(0));

        while (1)
        {
            // Enters the loop
            sleep(3); // Waits 5 seconds

            int i;
            for (i = 0; i < n_bots; i++)
            {
                // Generate random directions for bots to move
                rand_number = (rand() % 4) + 1;
                sprintf(s1, "%d", rand_number);

                if (i == 0)
                {
                    strcpy(s2, s1);
                }
                else
                {
                    strcat(s2, s1);
                }
            }
            // Encodes the directions into a long int variable
            long int dirs = atol(s2); // tipo ld

            // Send message with update directions to the server
            out_msg = msg2send(bot_mov, bot_client_pid, UNUSED_CHAR, -1, -1, dirs, -1);
            sendto(bot_client_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr *)&server_address, sizeof(server_address));
        }
    }
}