#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>  
#include <unistd.h>
#include <ncurses.h>
#include <time.h>
#include <ctype.h>
#include <pthread.h>
#include <stddef.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include<signal.h>

int aux;

void sig_handler(int signum){

  printf("Inside handler function %d\n",aux);
  exit(-1);
}

int main(){

    signal(SIGALRM,sig_handler); // Register signal handler

    alarm(5);  // Scheduled alarm after 2 seconds
    // alarm(0);  // Cancelled the previous alarm
    int key;
    aux = 1;
    while(1) {
      key = getchar();

      if(key==10) {
        alarm(0);
        printf("Alarm stopped\n");
        break;
      }
    }

return 0;
}