#include <stdio.h>
#include <pthread.h>  
#include <unistd.h> 
#include <stdlib.h>

void * thread_function(void * arg){
    int n = 0;
    long int r = random()%100;

    //Ex5
    int int_val = *(int *) arg;

    printf("Start Thread %lu\t Delay: %d\n", r, int_val);
    while(n <10){
        sleep(int_val);
        n++;
        printf("inside Thread %lu %d\n", r, n);
    }
    printf("End Thread %lu\n", r);
    return (void *)r;
}

int main(){
    char line[100];
    int n_threads;
    printf("How many threads: ");
    fgets(line, 100, stdin);
    sscanf(line, "%d", &n_threads);
    pthread_t thread_id[n_threads];

    //Ex4
    void * thread_ret;
    int ret_val;

    //Ex5
    int * thr_val = malloc(sizeof(int));


    int i = 0;
    while( i < n_threads) {
        *thr_val = random()%5;
        pthread_create(&thread_id[i], NULL,thread_function, thr_val);
        i++;
    };

    i=0;
    while (i < n_threads){
        //Ex3
        pthread_join(thread_id[i], &thread_ret);
        ret_val = (int) thread_ret;
        i++;
        printf("Thread %d returned value %d\n",i,ret_val);
    }

    printf("Carregue em enter para terminar\n");
    fgets(line, 100, stdin);
    exit(0);
}