#include "chase2.h"

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
        printf("Error: MAX bots allowed is 10!.\n");
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

    message_request out_msg;
    message_request in_msg;
    out_msg.type = bot_con;
    out_msg.n_bots = atoi(argv[2]); // como passo isto para int para poder guardar
    printf("N_bots = %d\n", atoi(argv[2]));

    // sendto(bot_client_sock, &out_msg, sizeof(message_request), 0, (struct sockaddr *)&server_address, sizeof(server_address));

    // recvfrom(bot_client_sock, &in_msg, sizeof(message_request), 0, (struct sockaddr *)&server_address, sizeof(server_address));

    char bot_dirs_array[n_bots];

    while (1)
    {
        // Gera as direções
        char *msg = (char *)malloc(sizeof(char) * BUFFER_SIZE + 1);
        int i;
        for (i = 0; i < n_bots; i++)
        {
            msg[i] = rand() % 4;
        }
        msg[i] = '\0';

        printf("Bots directions are is: [%d, %d, %d, %d]\n", msg[0], msg[1], msg[2], msg[3]);

        // out_msg.type = bot_dirs;
        // out_msg.direction =

        // sendto(bot_client_sock, &out_msg, sizeof(message_request), 0, (struct sockaddr *)&server_address, sizeof(server_address));

        sleep(3); // waits 3 seconds
    }
}
