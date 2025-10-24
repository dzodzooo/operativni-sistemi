#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

pthread_t t;
pthread_mutex_t mutex;
pthread_cond_t cond;
int n;
bool can_read;

	
void* routine(void*){

	while(1){
		pthread_mutex_lock(&mutex);
		while(!can_read){
			pthread_cond_wait(&cond, &mutex);
		}
		if(n==999){
			can_read=false;
			pthread_cond_signal(&cond);
			pthread_mutex_unlock(&mutex);
			break;
		}
		for(int i=0;i<=n;i++){
			printf("%d\n",i);
			sleep(2);
		}
		can_read=false;
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}
void main(){
	can_read=false;

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	pthread_create(&t, NULL, routine, NULL);
	while(1){
		pthread_mutex_lock(&mutex);
		while(can_read){
			pthread_cond_wait(&cond, &mutex);
		}
		printf("Unesi ceo broj:\n");
		scanf("%d", &n);
		if(n==999){
			can_read=true;
			pthread_cond_signal(&cond);
			pthread_mutex_unlock(&mutex);
			break;	
		}
		can_read=true;
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
			
	}
	pthread_join(t, NULL);
	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mutex);
}
