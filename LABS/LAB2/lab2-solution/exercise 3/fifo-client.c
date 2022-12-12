#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
int main(){


	int fd;
	char str[100];
	int n;
	while((fd = open("/tmp/fifo_teste", O_WRONLY))== -1){
	  if(mkfifo("/tmp/fifo_teste", 0666)!=0){
			printf("problem creating the fifo\n");
			exit(-1);
	  }else{
		  printf("fifo created\n");
	  }
	}
	printf("fifo just opened for writing\n");


	while(1){

		printf("write the name of the function");
		fgets(str, 100, stdin);
		str[strlen(str)-1] = '\0';
		write(fd, str, 100);
		printf("just send the functino name %s\n", str);


	}

}
