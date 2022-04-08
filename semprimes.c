#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>
#include <sempahores.h>

#define NTHREADS 4

int initial, final;
int prime_count[ NTHREADS ];

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

void* tfunc(void* args){
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
}


int main(){
    long long start_ts;
	long long stop_ts;
	long long elapsed_time;
    struct timeval ts;
	
	pthread_t *tid;
	int *args;

    int i;

	tid = malloc(NTHREADS * sizeof(pthread_t));
	args = malloc(NTHREADS * sizeof(int));

    printf("Enter initial value: \n");
	scanf("%d", &initial);
	printf("Enter final number: \n");
	scanf("%d", &final);

    gettimeofday(&ts, NULL);
	start_ts = ts.tv_sec; // Tiempo inicial

	for(i = 0; i < NTHREADS; i++){
		args[i] = i;
		pthread_create(&tid[i], NULL, tfunc, &args[i]);
	}
	
	for(i = 0; i<NTHREADS; i++)
		pthread_join(tid[i], NULL);

    gettimeofday(&ts, NULL);
	stop_ts = ts.tv_sec; // Tiempo final
	elapsed_time = stop_ts - start_ts;

	int total = 0;
	for(i = 0;i < NTHREADS; i++) {
		total += prime_count[ i ];
    }

	printf("%d\n", total);

    printf("------------------------------\n");
	printf("TIEMPO TOTAL, %lld segundos\n",elapsed_time);

    return 0;
}
