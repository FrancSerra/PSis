#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
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


	while(1){

		printf("write the name of the function");
		fgets(str, 100, stdin);
		str[strlen(str)-1] = '\0';
		write(fd, str, 100);
		printf("just send the functino name %s\n", str);

		read(fd2, &res, sizeof(res));
		printf("result of function %s -> %d\n", str, res);
		if(res == -1){
			printf("function %s not found\n", str);
		}

	}

}
