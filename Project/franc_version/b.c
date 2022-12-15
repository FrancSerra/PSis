#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_BOTS 11

int main () {
int rand_number;
char s1[2];
char s2[MAX_BOTS+1];

int i;
for(i=0;i<10;i++){
	
	rand_number = rand()%4;
    printf("%d\n", rand_number);
	sprintf(s1,"%d",rand_number);

	if(i==0){
		strcpy(s2,s1);
	}
	else{
		strcat(s2,s1);
	}
}

long int final_cat=atol(s2);
printf("%ld\n", final_cat);

}