#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include "semaphores.h"

#define FINDERS 4

int initial, final;
int* prime_number_end;

int mutex;
int sem_display;
int sem_next;

typedef struct node{
    int data;
    struct node *ptr;
} node;

node* head, *p_head;

node* insert(node* head, int num) {
    node *temp, *prev, *next;
    temp = (node*)malloc(sizeof(node));
    temp->data = num;
    temp->ptr = NULL;
    if(!head){
        head=temp;
    } else{
        prev = NULL;
        next = head;
        while(next && next->data<=num){
            prev = next;
            next = next->ptr;
        }
        if(!next){
            prev->ptr = temp;
        } else{
            if(prev) {
                temp->ptr = prev->ptr;
                prev-> ptr = temp;
            } else {
                temp->ptr = head;
                head = temp;
            }            
        }   
    }
    return head;
}

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

void display(){
	//printf("%d\n", primeNumber);
	while(prime_number_end[1] != FINDERS){
		semwait(sem_display);
		if(prime_number_end[1] != FINDERS){
			int _primeNumber = prime_number_end[0];
			head = insert(head, _primeNumber);
		}
		//printf("num primo: %d\n", prime_number_end[0]);
		semsignal(sem_next);
	}
	p_head = head;
	while(p_head){
		printf("%d\n", p_head->data);
		p_head = p_head->ptr;
	}
	exit(0);
}

void finder(int finderNumber){ 
	int SIZE = final - initial;
	int _initial = ( finderNumber * (SIZE / FINDERS)) + initial;
	int _final = _initial + (SIZE / FINDERS);

	//printf("%d\n", finderNumber);

	for( int i = _initial; i < _final; i++){
		if(isprime(i)){
			semwait(mutex);
			prime_number_end[0] = i;
			semsignal(sem_display);
			//printf("%d\n", prime_count);
			semwait(sem_next);
			semsignal(mutex);

		}
	}
	semwait(mutex);
	prime_number_end[1]++;
	semsignal(mutex);
	exit(0);
}

int main(int argc, char *argv[]){
    /*long long start_ts;
	long long stop_ts;
	long long elapsed_time;
    struct timeval ts;*/
	int shmid;

    int i;

	mutex = createsem(0x1200, 1);
	sem_display = createsem(0x1225, 0);
	sem_next = createsem(0x1250, 0);

    initial = atoi(argv[1]);
	final = atoi(argv[2]);

    /*gettimeofday(&ts, NULL);
	start_ts = ts.tv_sec; // Tiempo inicial*/

	shmid = shmget(0x1500, 2 * sizeof(int), IPC_CREAT | 0660);
	if(shmid < 0){
		fprintf(stderr, "Error al obtener memoria compartida\n");
		exit(1);
	}
	prime_number_end = shmat(shmid, NULL, 0);
	prime_number_end[1] = 0;


	if(fork() == 0){
		display();
		exit(1);
	}

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

    /*gettimeofday(&ts, NULL);
	stop_ts = ts.tv_sec; // Tiempo final
	elapsed_time = stop_ts - start_ts;
	
    printf("------------------------------\n");
	printf("TIEMPO TOTAL, %lld segundos\n",elapsed_time);*/

    return 0;
}