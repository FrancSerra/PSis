#include "chase.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Error: Missing one argument.\n");
        exit(1);
    }
    int n_bots = atoi(argv[2]);
    if (n_bots < 0 || n_bots > 10)
    {
        printf("Error: Max bots allowed is 10!.\n");
        exit(1);
    }

    //////////// SOCKET CREATION /////////////
    int bot_client_sock;
    struct sockaddr_un server_address, bot_client_address;

    bot_client_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (bot_client_sock == -1)
    {
        perror("socket: ");
        exit(-1);
    }

    // Geração do ID do bot_client
    int bot_client_pid;
    bot_client_pid = getpid(); ///////

    bot_client_address.sun_family = AF_UNIX;
    sprintf(bot_client_address.sun_path, "/tmp/client%d", bot_client_pid);

    unlink(bot_client_address.sun_path);
    int err = bind(bot_client_sock, (const struct sockaddr *)&bot_client_address, sizeof(bot_client_address));
    if (err == -1)
    {
        perror("bind");
        exit(-1);
    }

    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, argv[1]);

    //////////////////////////////////////////

    message_t out_msg, in_msg;
    printf("N_bots = %d\n", n_bots);

    //                                                                    n_bots is stored in health parameter
    out_msg = msg2send(bot_conn, bot_client_pid, UNUSED_CHAR, -1, -1, -1, n_bots);
    sendto(bot_client_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr *)&server_address, sizeof(server_address));
    printf("Connect message sent\n");

    recvfrom(bot_client_sock, &in_msg, sizeof(message_t), 0, (struct sockaddr *)&server_address, sizeof(server_address));
    if (in_msg.type != bot_conn)
    {
        printf("Failed connecting\nTry again!\n");
        exit(-1);
    }
    else
    {
        printf("Bots running \n");

        // WHILE LOOP HERE!!

        while (1)
        {

            // vvvvv Gerar mensagens AQUI vvvvv//

            // ^^^^^ Gerar mensagens AQUI ^^^^^//

            // Send Message

            // sendto(bot_client_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr *)&server_address, sizeof(server_address));

            // waits 3 seconds

            sleep(3); // NÃO ESTÁ A FUNCIONAR O LOOP!
            printf("Message Sent\n");
            fflush(stdout);
        }
    }
}
