#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include "semaphores.h"
#include "linkedList.h"

#define FINDERS 4

int initial, final;
int* prime_count_number;

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
	//printf("%d\n", primeNumber);
	prime_count_number[0] = primeNumber;
	
	if(prime_count_number[0]){
		push(prime_count_number[0]);
		printlist(head);
		printf("\n");
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

	//printf("%d\n", finderNumber);

	for( int i = _initial; i < _final; i++){
		if(isprime(i)){
			semwait(mutex);
			prime_count_number[1]++;
			//printf("%d\n", prime_count);
	
			display(i);
			semsignal(mutex);

		}
	}

}

int main(int argc, char *argv[]){
    long long start_ts;
	long long stop_ts;
	long long elapsed_time;
    struct timeval ts;
	int shmid;

    int i;


	mutex = createsem(0x1200, 1);
	sem_display = createsem(0x1201, 0);

    initial = atoi(argv[1]);
	final = atoi(argv[2]);

    gettimeofday(&ts, NULL);
	start_ts = ts.tv_sec; // Tiempo inicial

	shmid = shmget(0x1500, 2 * sizeof(int), IPC_CREAT | 0666);
	if(shmid < 0){
		fprintf(stderr, "Error al obtener memoria compartida\n");
		exit(1);
	}
	prime_count_number = shmat(shmid, NULL, 0);
	prime_count_number[1] = 0;


	for(int i = 0; i < FINDERS; i++){
		if(fork() == 0){
			finder(i);
			exit(1);
		}
	}

	for(int i = 0; i < FINDERS; i++){
		wait(NULL);
	}

	

	erasesem(mutex);
	erasesem(sem_display);


    gettimeofday(&ts, NULL);
	stop_ts = ts.tv_sec; // Tiempo final
	elapsed_time = stop_ts - start_ts;

	int total = prime_count_number[1];

	printf("%d\n", total);

    printf("------------------------------\n");
	printf("TIEMPO TOTAL, %lld segundos\n",elapsed_time);

    return 0;
}
