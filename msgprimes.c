#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/msg.h>

#define FINDERS 4

int initial, final;

struct msgtype{
	long msg_type;
	int prime_number;
};

int msgqid;

typedef struct node{
    int data;
    struct node *ptr;
} node;

node* head, *p_head;

node* push(node* head, int num) {
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

    struct msgtype mensaje;
	
    int ended_finders;

	while(ended_finders != FINDERS){
		 msgrcv(msgqid, &mensaje, sizeof(struct msgtype), 2, 0);
		if(mensaje.prime_number == -1){
            ended_finders++;
        }
        else{
            int _primeNumber = mensaje.prime_number;
            head = push(head, _primeNumber);
        }
	}
	p_head = head;
	while(p_head){
		printf("%d\n", p_head->data);
		p_head = p_head->ptr;
	}
	exit(0);
}

void finder(int finderNumber){ 
    struct msgtype mensaje;

	int SIZE = final - initial;
	int _initial = ( finderNumber * (SIZE / FINDERS)) + initial;
	int _final = _initial + (SIZE / FINDERS);

	//printf("%d\n", finderNumber);

	for( int i = _initial; i < _final; i++){
		if(isprime(i)){
			mensaje.msg_type = 2;
            mensaje.prime_number = i;
            msgsnd(msgqid, &mensaje, sizeof(struct msgtype), 0);
		}
	}
	mensaje.prime_number = -1;
    msgsnd(msgqid, &mensaje, sizeof(struct msgtype), 0);
	exit(0);
}

int main(int argc, char *argv[]){

    int i;

    initial = atoi(argv[1]);
	final = atoi(argv[2]);

    msgqid = msgget(0x2000, 0666 | IPC_CREAT);
	
	if(msgqid == -1){
		fprintf(stderr, "Error al crear el buzon\n");
	}

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
	msgctl(msgqid, IPC_RMID, NULL);

    return 0;
}