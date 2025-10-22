#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

pthread_cond_t is_divisible_by_7;
pthread_mutex_t lock;
int number;
int i;
void* do1(void*){
	while(i<number){
		pthread_mutex_lock(&lock);
		while(i%7!=0){
			pthread_cond_wait(&is_divisible_by_7, &lock);
		}
		if(i<=number)
			printf("*%d*\n",i);
		else
		{
			pthread_mutex_unlock;
			break;
		}
		i++;
		pthread_cond_signal(&is_divisible_by_7);
		pthread_mutex_unlock(&lock);
	}
}
void* do2(void*){
	while(i<number){
		pthread_mutex_lock(&lock);
		while(i%7==0){
			if(i==number) break;
			pthread_cond_wait(&is_divisible_by_7, &lock);
		}
		if(i<=number)
			printf("*%d*\n",i);
		else{
			pthread_mutex_unlock;
			break;
		}
		i++;
		pthread_cond_signal(&is_divisible_by_7);
		pthread_mutex_unlock(&lock);
	}
}

void main(int argc, char** argv){
	if(argc!=2){
		fprintf(stderr,"Usage ./main number");
		exit(EXIT_FAILURE);
	}
	
	pthread_cond_init(&is_divisible_by_7, NULL);
	pthread_mutex_init(&lock, NULL);
	
	pthread_mutex_lock(&lock);
	number=atoi(argv[1]);
	printf("input=%d\n", number);
	i=0;
	pthread_mutex_unlock(&lock);
	
	pthread_t t1, t2;
	pthread_create(&t1, NULL, do1, NULL);
	pthread_create(&t2, NULL, do2, NULL);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&is_divisible_by_7);
}
