#include "chase.h"

int main()
{
    int server_sock;
    struct sockaddr_un server_address, client_address;
    int client_address_size = sizeof(client_address);

    server_sock = socket(AF_UNIX, SOCK_DGRAM, 0);

    if (server_sock == -1)
    {
        perror("socket: ");
        exit(-1);
    }

    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, SERVER_ADDRESS);

    unlink(SERVER_ADDRESS);
    int err = bind(server_sock, (struct sockaddr *)&server_address, sizeof(server_address));

    if (err == -1)
    {
        perror("bind");
        exit(-1);
    }

    message_t in_msg, out_msg;
    int n_bytes;

    //////////// DECLARAÇÃO DAS LISTAS////////////
    client_list *head = create_head_client_list();
    client_list *player;
    ///////////////////////////////////////////

    num_players = 0;

    int new_client_err, delete_client_err;
    position_t init_pos;

    WINDOW *my_win = generate_window();
    draw_health(NULL, 0, false);
    int client_pid;

    while (1)
    {
        n_bytes = recvfrom(server_sock, &in_msg, sizeof(message_t), 0, (struct sockaddr *)&client_address, &client_address_size);

        // Obtem o ID
        client_pid = atoi((client_address.sun_path + strlen("/temp/client") - 1));

        if (n_bytes == sizeof(message_t))
        {
            switch (in_msg.type)
            {
            case conn:
                if (num_players < MAX_PLAYERS)
                {
                    num_players++;
                    init_pos = initialize_player(head);
                    new_client_err = insert_new_client(head, client_pid, init_pos.c, init_pos.x, init_pos.y, INITIAL_HEALTH);
                    if (new_client_err != -1)
                    { // caso seja adicionado à lista com sucesso
                        draw_player(my_win, &init_pos, true);
                        draw_health(&init_pos, 1, false);
                        out_msg = msg2send(ball_info, client_pid, init_pos.c, init_pos.x, init_pos.y, -1, INITIAL_HEALTH);
                    }
                    else
                    { // caso haja erro de alocacao de memoria
                        out_msg = msg2send(error, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
                    }
                }
                else
                { // caso já haja 10 players
                    out_msg = msg2send(error, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
                }
                sendto(server_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr *)&client_address, sizeof(client_address));
                break;

                /////////////////NÃO FUNCIONA! em baixo está a original/////////////////
                // case ball_mov:

                //     player = update_client(head, client_pid, in_msg.direction, my_win);

                //     if (player == NULL)
                //     {
                //         out_msg = msg2send(error, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
                //     }
                //     else if (player->health == 0)
                //     {
                //         // define mensagem de fim de jogo (morreu) health0
                //         out_msg = msg2send(health0, client_pid, UNUSED_CHAR, -1, -1, -1, -1);

                //         // apaga o jogador da lista
                //         delete_client_err = delete_client(head, client_pid, my_win);
                //         if (delete_client_err == -1)
                //         {
                //             printf("This client was not yet connected.\n");
                //         }
                //         else
                //         {
                //             num_players--;
                //         }
                //     }

                // sendto(server_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr *)&client_address, sizeof(client_address));
                // break;
                /////////////////NÃO FUNCIONA/////////////////

            case ball_mov:
                player = update_client(head, client_pid, in_msg.direction, my_win);
                if (player == NULL)
                {
                    out_msg = msg2send(error, client_pid, UNUSED_CHAR, -1, -1, -1, -1);
                }
                else
                {
                    // field_status
                }
                break;

            case disconn:
                delete_client_err = delete_client(head, client_pid, my_win);
                if (delete_client_err == -1)
                {
                    printf("This client was not yet connected.\n");
                }
                else
                {
                    num_players--;
                }
                break;

            case bot_conn:
                // Tells bot client that he can star sending messages
                out_msg.type = bot_conn;
                sendto(server_sock, &out_msg, sizeof(message_t), 0, (struct sockaddr *)&client_address, sizeof(client_address));

                int n_bots = in_msg.health; // HEALTH parameter is the carrier for n_bots info. It's the way it was defined

                for (int i = 0; i < n_bots; i++)
                {
                    // Initializes bot
                    init_pos = initialize_bot(n_bots, head);

                    //                                                              health doesn't have a meaning for the bot
                    new_client_err = insert_new_client(head, i + 1, init_pos.c, init_pos.x, init_pos.y, -1);
                    draw_player(my_win, &init_pos, true);
                }
                break;

            default:
                break;
            }
        }
        else
        {
            perror("Fails to receive message.\n");
        }
    }
}