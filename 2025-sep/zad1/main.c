#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

#define MAGIC_NUMBER 4
#define TIME_PERIOD 1 

int buff[MAGIC_NUMBER];
int generated_nums;
pthread_t thread;
pthread_mutex_t mutex;
pthread_cond_t cond;
bool needs_to_work;

void* generate_nums(void* args){
	while(1)
	{
		pthread_mutex_lock(&mutex);
		if(!needs_to_work){
			pthread_mutex_unlock(&mutex);
			break;	
		}
		for(generated_nums=0; generated_nums<MAGIC_NUMBER; generated_nums++){
			buff[generated_nums]=rand()%99;
		}
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
		sleep(TIME_PERIOD);
	}
}

void main(int argc, char** argv){
	srand(time(NULL));
	needs_to_work=true;
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);
	pthread_create(&thread, NULL, generate_nums, NULL);
	int sum=0;
	while(sum<=250){
		pthread_mutex_lock(&mutex);
		while(generated_nums<MAGIC_NUMBER)
		{
			pthread_cond_wait(&cond, &mutex);
		}

		int tmp=0;
		for(int i=0; i<MAGIC_NUMBER; i++){
			tmp+=buff[i];	
		}
		if(tmp<50){
			printf("Suma je %d\n", tmp);
		}

		sum+=tmp;
		pthread_mutex_unlock(&mutex);
	}
	needs_to_work=false;
	printf("Krajnja suma je: %d\n", sum);
	pthread_join(thread, NULL);
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);

}

