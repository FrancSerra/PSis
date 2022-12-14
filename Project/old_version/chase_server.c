#include "chase2.h"

position_t p1; // Prof
client_list *head;

int main()
{
    //////////// SOCKET CREATION /////////////
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
    //////////////////// WINDOW //////////////////////////
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();

    /* creates a window and draws a border */
    WINDOW *my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(my_win, 0, 0);
    wrefresh(my_win);
    /* creates a MESSAGE window and draws a border */
    WINDOW *message_win = newwin(5, WINDOW_SIZE, WINDOW_SIZE, 0);
    box(message_win, 0, 0);
    wrefresh(message_win);
    /////////////////////////////////////////////

    //////////////////////////////////////////////

    int n_bytes;

    head = create_head_client_list(); // starts the list where we will kepp clients
    num_players = 0;

    int new_client_err, delete_client_err;
    position_t init_pos;

    int health;
    int ch;                // these are
    int pos_x;             // for the
    int pos_y;             // ball_move
    direction_t direction; // messages
    int client_pid;
    client_list *current_player; // ponteiro para estrutura da lista de clientes

    message_request in_msg;
    message_ball_info out_msg;
    char *out_msg2;

    position_t array_update[100];
    position_t *array_ptr = array_update; // inicializa o ponteiro para o primeiro elemento

    while (1)
    {
        n_bytes = recvfrom(server_sock, &in_msg, sizeof(message_request), 0, (struct sockaddr *)&client_address, &client_address_size);

        // mvwprintw(message_win, 1, 1, "CLI: %s\n", client_address.sun_path);
        // wrefresh(message_win);

        if (n_bytes == sizeof(message_request))
        {

            switch (in_msg.type)
            {
            case conn:
                if (num_players < MAX_PLAYERS)
                {
                    num_players++;

                    init_pos = initialize_player(head); // estrutura com x y e ch

                    /* draw mark on new position */
                    wmove(my_win, init_pos.x, init_pos.y);
                    waddch(my_win, init_pos.c | A_BOLD);
                    wrefresh(my_win);

                    // insert the new client in the list
                    new_client_err = insert_new_client(head, in_msg.pid, init_pos.c, init_pos.x, init_pos.y, INITIAL_HEALTH);
                    if (new_client_err != -1)
                    { // caso seja adicionado à lista com sucesso
                        out_msg.pid = in_msg.pid;
                        out_msg.c = init_pos.c;
                        out_msg.x = init_pos.x;
                        out_msg.y = init_pos.y;
                    }
                    else
                    {                  // caso haja erro de alocacao de memoria
                        out_msg.c = 0; // Not alpha
                    }
                }
                else
                {                  // caso já haja 10 players
                    out_msg.c = 0; // Not alpha  gonna give error
                }

                sendto(server_sock, &out_msg, sizeof(message_ball_info), 0, (struct sockaddr *)&client_address, sizeof(client_address));
                break;

            case disconn:
                delete_client_err = delete_client(head, in_msg.pid);
                if (delete_client_err == -1)
                {
                    printf("This client was not yet connected.\n");
                }
                else
                {
                    num_players--;
                }
                break;

            case ball_mov:
                client_pid = in_msg.pid;

                // ir a lista buscar o player atual.
                current_player = search_player(head, client_pid);

                pos_x = current_player->x;
                pos_y = current_player->y;
                health = current_player->health;

                // get the direction where to move
                direction = in_msg.direction;

                // deletes old place
                wmove(my_win, pos_x, pos_y);
                waddch(my_win, ' ');

                // calculates new mark position
                // new_position(&pos_x, &pos_y, direction);

                // Store again the parameters in the list in the list
                // #################HEALTH AINDA NÃO FOI ATUALIZADA

                if (health <= 0)
                {
                    out_msg2 = constructor(health0, head);
                }
                else
                {
                    current_player->x = pos_x;

                    current_player->y = pos_y;

                    current_player->health = health;

                    out_msg2 = constructor(field_stat, head);
                }

                sendto(server_sock, out_msg2, sizeof(char) * (BUFFER_SIZE + 1), 0, (struct sockaddr *)&client_address, sizeof(client_address));
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
