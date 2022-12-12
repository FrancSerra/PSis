#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>


int main(){


	int fd;
	int fd2;


	// loading the library
	char library_name[100];
	sprintf(library_name, "./funcs.so");
	void * lib_handler = dlopen(library_name, RTLD_LAZY);
	if (lib_handler == NULL){
		printf("library %s not found\n", "library_name");
		exit(-1);
	}else{
		printf("library %s loaded\n", library_name);
	}



	while((fd = open("/tmp/fifo_teste", O_RDONLY))== -1){
	  if(mkfifo("/tmp/fifo_teste", 0666)!=0){
			printf("problem creating the fifo\n");
			exit(-1);
	  }else{
		  printf("fifo created\n");
	  }
	}
	printf("fifo just opened\n");


	while((fd2 = open("/tmp/fifo_response", O_WRONLY))== -1){
	  if(mkfifo("/tmp/fifo_response", 0666)!=0){
			printf("problem creating the fifo 2 \n");
			exit(-1);
	  }else{
		  printf("fifo 2 created\n");
	  }
	}
	printf("fifo 2  just opened for writing\n");


	int n;
	char str[100];
	int (*funct)();
	int res;
	
	while(1){

		printf("waiting for a message ");
		n = read(fd, str, 100);
		if(n<=0){
			perror("read ");
			exit(-1);
		}
		printf("\nJust received the function names %s (%d bytes)\n", str, n);

		funct = dlsym(lib_handler, str);
		if(funct == NULL){
			printf("Function %s not found\n", str);
			res = -1;
			write(fd2, &res, sizeof(res));
		}else{
			printf("Executinf %s function...\n", str);
			res = funct();
			printf("result of  %s function -> %d \n", str, res);
			write(fd2, &res, sizeof(res));
		}		
	}

	printf("fifo aberto\n");

}
