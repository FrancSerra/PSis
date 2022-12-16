#include "chase.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Error: Missing recipient IP.\n");
        exit(1);
    }

    int prizes_client_sock;
    struct sockaddr_un server_address, prizes_client_address;
    int server_address_size = sizeof(server_address);

    prizes_client_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (prizes_client_sock == -1)
    {
        perror("socket: ");
        exit(-1);
    }

    prizes_client_address.sun_family = AF_UNIX;
    sprintf(prizes_client_address.sun_path, "/tmp/client%d", getpid());

    unlink(prizes_client_address.sun_path);
    int err = bind(prizes_client_sock, (const struct sockaddr *)&prizes_client_address, sizeof(prizes_client_address));
    if (err == -1)
    {
        perror("bind");
        exit(-1);
    }

    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, argv[1]);

    int prizes_client_pid;
    prizes_client_pid = getpid();

    message_t in_msg, out_msg;
    int n_bytes, n_prizes;

    n_prizes = INIT_PRIZES;
    out_msg = msg2send(prizes_conn, prizes_client_pid, UNUSED_CHAR, -1, -1, -1, 0); // health used as flag: 0 means initialize 5 prizes
    sendto(prizes_client_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr *)&server_address, sizeof(server_address));
    printf("Connect message sent\n");

    n_bytes = recvfrom(prizes_client_sock, &in_msg, sizeof(message_t), 0, (struct sockaddr *)&server_address, (socklen_t *)&server_address_size);

    if (n_bytes != sizeof(message_t))
    {
        printf("Failed communication.\nYou have been disconnected.\n");
        exit(-1);
    }

    if (in_msg.type != prizes_conn)
    {
        printf("Failed connecting.\n");
        exit(-1);
    }
    else
    {
        printf("Added %d out of %d prizes.\n", in_msg.health, n_prizes);
        n_prizes = PRIZES_LOOP;
        printf("Prizes running...\n");
        while (1)
        {
            sleep(5);
            out_msg = msg2send(prizes_conn, prizes_client_pid, UNUSED_CHAR, -1, -1, -1, 1); // health used as flag: 1 means put one new prize
            sendto(prizes_client_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr *)&server_address, sizeof(server_address));
            // printf("Message sent.\n");
            fflush(stdout);
            n_bytes = recvfrom(prizes_client_sock, &in_msg, sizeof(message_t), 0, (struct sockaddr *)&server_address, (socklen_t *)&server_address_size);

            if (n_bytes != sizeof(message_t))
            {
                printf("Failed communication.\n");
            }

            else
            {
                if (in_msg.type != prizes_conn)
                {
                    printf("Failed connecting.\n");
                }
                else
                {
                    // printf("Added %d out of %d prize.\n", in_msg.health, n_prizes);
                }
            }
        }
    }
}