#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>


int main(){


	int fd;



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


	int n;
	char str[100];
	int (*funct)();

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
		}else{
			printf("Executinf %s function...\n", str);
			int res = funct();
			printf("result of  %s function -> %d \n", str, res);
		}		
	}

	printf("fifo aberto\n");

}
