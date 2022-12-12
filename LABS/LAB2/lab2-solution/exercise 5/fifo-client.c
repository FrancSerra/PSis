#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "data_struct.h"

int main(){


	int fd;
	int fd2;
	char str[100];
		
	int n;
	int res;

	while((fd = open("/tmp/fifo_teste", O_WRONLY))== -1){
	  if(mkfifo("/tmp/fifo_teste", 0666)!=0){
			printf("problem creating the fifo\n");
			exit(-1);
	  }else{
		  printf("fifo created\n");
	  }
	}
	printf("fifo just opened for writing\n");



	while((fd2 = open("/tmp/fifo_response", O_RDONLY))== -1){
	  if(mkfifo("/tmp/fifo_response", 0666)!=0){
			printf("problem creating the fifo 2 \n");
			exit(-1);
	  }else{
		  printf("fifo 2 created\n");
	  }
	}
	printf("fifo 2  just opened for reading\n");

	message_type msg;

	while(1){

		printf("what type of function int: \n1 - int (*)() \n2 - int (*)(int)\n");
		fgets(str, 100, stdin);
		sscanf(str, "%d", &msg.funct_type);
		if(msg.funct_type != 1 && msg.funct_type != 2){
			continue;
		}
		printf("write the name of the function");
		fgets(msg.f_name, 100, stdin);
		msg.f_name[strlen(msg.f_name)-1] = '\0';

		if (msg.funct_type == 2){
			printf("type the function %s integer argument: ", msg.f_name);
			fgets(str, 100, stdin);
			sscanf(str, "%d", &msg.arg);
		}

		write(fd, &msg, sizeof(msg));
		printf("just send the function name %s\n", msg.f_name);

		read(fd2, &res, sizeof(res));
		printf("result of function %s -> %d\n", msg.f_name, res);
		if(res == -1){
			printf("function %s not found\n", msg.f_name);
		}

	}

}
