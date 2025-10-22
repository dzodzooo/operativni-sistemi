#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

int NUMBER;
pthread_cond_t countdown_ready, userinput_ready;
pthread_mutex_t mutex;
bool can_read_number, userinput_is_ready;

void count_down_from_NUMBER(){
	while(NUMBER!=0)
	{
		pthread_mutex_lock(&mutex);
		while(!userinput_is_ready)
		{
			pthread_cond_wait(&countdown_ready, &mutex);
		}
		for(int i=NUMBER;i>0;i--)
		{
			printf("%d\n",i);
			sleep(4);
		}
		printf("Counting done.\n");

		can_read_number=true;
		userinput_is_ready=false;
		pthread_cond_signal(&userinput_ready);
		pthread_mutex_unlock(&mutex);
	}
	printf("done\n");
}

void init_env(){
	NUMBER=-1;
	can_read_number=true;
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&userinput_ready,NULL);
	pthread_cond_init(&countdown_ready,NULL);
}
void cleanup(){
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&userinput_ready);
	pthread_cond_destroy(&countdown_ready);
}

void main(int argc, char** argv){
	init_env();
	pthread_t thread;
	pthread_create(&thread, NULL, (void*)count_down_from_NUMBER, NULL);
	while(NUMBER!=0){
		pthread_mutex_lock(&mutex);
		while(!can_read_number)
		{
			pthread_cond_wait(&userinput_ready, &mutex);
		}
		printf("Uneti pozitivan broj: ");
		scanf("%d", &NUMBER);
		can_read_number=false;
		userinput_is_ready=true;
		pthread_cond_signal (&countdown_ready);
		pthread_mutex_unlock(&mutex);
	}
	printf("done reading numbers.\n");
	pthread_join(thread,NULL);
	cleanup();
}
