#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <sys/wait.h>
#include "semaphores.h"
#include "linkedList.h"


#define FINDERS 4
#define DISPLAY 1

int initial, final;
int prime_count = 0;
//struct node* head = NULL;

int mutex;
int sem_display;

int isprime(int n){
	int d=3;
	int prime=0;
	int limit=sqrt(n);
	
	if(n<2)
		prime=0;
	else if(n==2)
		prime=1;
	else if(n%2==0)
		prime=0;
	else
	{
		while(d<=limit && n%d)
			d+=2;
		prime=d>limit;
	}
	return(prime);
}

/*void* tfunc(void* args){
	int i;
	int threadNumber = *((int *) args);
	prime_count[ threadNumber ] = 0;

	int SIZE = final - initial;
	int _initial = ( threadNumber * ( SIZE / NTHREADS ) ) + initial;
	int _final = _initial + ( SIZE / NTHREADS );
	
    for(i = _initial;i < _final; i++){
    	if(isprime(i)){
        	prime_count[ threadNumber ]++;
        }
    }
}*/

void display(int primeNumber){
	if(primeNumber){
		push(primeNumber);
	}
	else{
		insertionsort(head);
		printlist(head);
	}
}

void finder(int finderNumber){ 
	int SIZE = final - initial;
	int _initial = ( finderNumber * (SIZE / FINDERS)) + initial;
	int _final = _initial + (SIZE / FINDERS);

	for( int i = _initial; i < _final; i++){
		if(isprime(i)){
			semwait(mutex);
			prime_count++;
			display(i);
			semsignal(mutex);

		}
	}

}



int
int main(char *argv[]){
    long long start_ts;
	long long stop_ts;
	long long elapsed_time;
    struct timeval ts;

    int i;

	mutex = createsem(0x1111, 1);
	sem_display = createsem(0x1112, 0);

    initial = atoi(argv[1]);
	final = atoi(argv[2]);

    gettimeofday(&ts, NULL);
	start_ts = ts.tv_sec; // Tiempo inicial

	for(int i = 0; i < FINDERS; i++){
		if(fork() == 0){
			finder(i);
			exit(1);
		}
	}

	for(int i = 0; i < FINDERS; i++){
		wait(NULL);
	}

	display(0);

	erasesem(mutex);
	erasesem(sem_display);


    gettimeofday(&ts, NULL);
	stop_ts = ts.tv_sec; // Tiempo final
	elapsed_time = stop_ts - start_ts;

	int total = prime_count;

	printf("%d\n", total);

    printf("------------------------------\n");
	printf("TIEMPO TOTAL, %lld segundos\n",elapsed_time);

    return 0;
}
