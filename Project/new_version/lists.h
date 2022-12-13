#include <stdlib.h>
#include <stdio.h>

typedef struct client_list{
    int pid;
    char c;
    int x, y;
    int health;
    struct client_list *next;
} client_list;


client_list* create_head_client_list();
int insert_new_client(client_list* head, int pid, char c, int x, int y, int health);
int delete_client(client_list* head, int pid);
void print_client_list(client_list *node);