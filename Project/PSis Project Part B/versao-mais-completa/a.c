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



#include <wchar.h>
#include <locale.h>

int aux;

void sig_handler(int signum){
  
  setlocale(LC_CTYPE, "");
  
  wchar_t c; 
  c = 0x2606;
  wprintf (L"Hello there: %lc\n", c);

  exit(-1);
}

int main(){

    signal(SIGALRM,sig_handler); // Register signal handler

    alarm(2);  // Scheduled alarm after 2 seconds
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