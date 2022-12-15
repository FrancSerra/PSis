#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int main() {
    // Seed the random number generator using the current time
    srand(time(NULL));

    char result[100];
    char x[100];

    

    int rand_int = (rand() % 4);
    sprintf(result1,"%d",rand_int);


    int i;
    // Generate and concatenate four random integers
    for (i = 0; i < 10; i++) {
        rand_int = (rand() % 4)+1;
        sprintf(x,"%d", rand_int);
        strcat(result, x);
    }
    printf("----------------------------\n");
    result[i] = '\0';
    
    printf("The string is %s\n", result);

    int final_int = atol(result);

    // Convert the result to an integer
    printf("The int is %d\n", final_int);
    printf("----------------------------\n");

    return 0;
}