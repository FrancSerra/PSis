#include "chase2.h"

WINDOW *message_win;

position_t p1; // Prof
client_list *head;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Error: Missing recipient IP.\n");
        exit(1);
    }
    printf("--1\n");
    /* creates a GAME window and draws a border */
    WINDOW *my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(my_win, 0, 0);
    wrefresh(my_win);
    keypad(my_win, true);
    /* creates a MESSAGE window and draws a border */
    message_win = newwin(5, WINDOW_SIZE, WINDOW_SIZE, 0);
    box(message_win, 0, 0);
    wrefresh(message_win);
    printf("ISSO a\n");

    //////////// SOCKET CREATION /////////////
    int client_sock;
    struct sockaddr_un server_address, client_address;

    client_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (client_sock == -1)
    {
        perror("socket: ");
        exit(-1);
    }
    // Geração do ID do cliente
    int client_pid;
    client_pid = getpid(); ///////

    client_address.sun_family = AF_UNIX;
    sprintf(client_address.sun_path, "/tmp/client%d", client_pid);

    unlink(client_address.sun_path);
    int err = bind(client_sock, (const struct sockaddr *)&client_address, sizeof(client_address));
    if (err == -1)
    {
        perror("bind");
        exit(-1);
    }

    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, argv[1]);
    //////////////////////////////////////////////

    message_request out_msg;
    message_ball_info in_msg;
    int n_bytes;

    /// Connection request
    out_msg.type = conn;
    out_msg.pid = client_pid;

    sendto(client_sock, &out_msg, sizeof(message_request), 0, (struct sockaddr *)&server_address, sizeof(server_address));

    printf("ISSO l\n");
    n_bytes = recvfrom(client_sock, &in_msg, sizeof(message_ball_info), 0, (struct sockaddr *)&server_address, sizeof(server_address));
    printf("ISSO m\n");

    // Answear Interpretation

    int health;
    if (n_bytes == sizeof(message_ball_info))
    {
        if (isalpha(in_msg.c))
        {
            p1.c = in_msg.c;
            p1.x = in_msg.x;
            p1.y = in_msg.y;
            p1.health = MAX_HEALTH;
        }
        else
        { // error
            exit(-1);
        }
    }

    //////////////////WINDOW//////////////////

    initscr();            /* Start curses mode 		*/
    cbreak();             /* Line buffering disabled	*/
    keypad(stdscr, TRUE); /* We get F1, F2 etc..		*/
    noecho();             /* Don't echo() while we do getch */

    /////////////////////////////////////////////

    // Draws player in place
    new_player(&p1, p1.c);
    draw_player(my_win, &p1, true);

    // mvwprintw(message_win, 1,1,"type: %d\n x: %d\n y:%d\n c: %c\n health:%d\n", in_msg.type, in_msg.x, in_msg.y, in_msg.c, in_msg.health);
    // wrefresh(message_win);

    ////////////////////////////////// MAIN LOOP /////////////////////////////////

    // onde lê o keyboard. envia a ball_movement
    // recebe a Field status ou health = 0; e dá update da janela

    int key;
    int n = 0;

    // message_request out_msg;     //continua deste tipo portanto não tenho que declarar de novo

    char in_msg2[BUFFER_SIZE + 1];

    printf("ISSO 0\n");
    // prepare the movement message
    out_msg.type = ball_mov;
    while (1)
    {
        printf("ISSO 1\n");

        // read the keyboard
        do
        {
            key = wgetch(my_win);
            n++;
            printf(".................\n");
            switch (key)
            {
            case KEY_LEFT:
                printf("%d Left arrow is pressed\n", n);
                // TODO_9
                //  prepare the movement message
                out_msg.direction = LEFT;
                break;
            case KEY_RIGHT:
                printf("%d Right arrow is pressed\n", n);
                // TODO_9
                //  prepare the movement message
                out_msg.direction = RIGHT;
                break;
            case KEY_DOWN:
                printf("%d Down arrow is pressed\n", n);
                // TODO_9
                //  prepare the movement message
                out_msg.direction = DOWN;
                break;
            case KEY_UP:
                printf("%d :Up arrow is pressed\n", n);
                // TODO_9
                //  prepare the movement message
                out_msg.direction = UP;
                break;
            case 'q':
                printf("QUIT is pressed\n", n);
                break;
            }

            // printf("KEY: %d\n", key);

        } while (1);

        // print the pressed key
        mvwprintw(message_win, 1, 1, "%c key pressed", key);
        wrefresh(message_win);

        // send the movement message
        sendto(client_sock, &out_msg, sizeof(message_request), 0, (struct sockaddr *)&server_address, sizeof(server_address));

        // Answear interpretation with either Field Status or helth_0 messages:
        n_bytes = recvfrom(client_sock, in_msg2, sizeof(char) * (BUFFER_SIZE + 1), 0, (struct sockaddr *)&server_address, sizeof(server_address));

        if (n_bytes == sizeof(char) * (BUFFER_SIZE + 1))
        {

            int type;
            char *info;

            sscanf(in_msg2, "%d&%s", &type, info);

            if (type == health0)
            {
                exit(-1);
            }
            else
            {
                char *clients, *bots, *prizes;
                sscanf(info, "%[^+]+%[^+]+%[^+]", clients, bots, prizes);
                mvwprintw(message_win, 1, 1, "%s", clients);
                wrefresh(message_win);
            }
        }
        else
        {
            // print the pressed key
            mvwprintw(message_win, 1, 1, "YOU DEAD");
            wrefresh(message_win);
            exit(-1);
            break;
        }
    }

    mvwprintw(message_win, 1, 1, "%c key pressed", key);
    wrefresh(message_win);
}

////////////////////////////////END OF MAIN LOOP /////////////////////////////////////////////
