#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_t t_even;
pthread_t t_odd;
int buff[10];

pthread_mutex_t mutex[2]; 
pthread_cond_t cond[2];
int ready[2];

void* generate(void* arguments){
	int* args=(int*) arguments;
	int parity = args[0];
	int start_value=args[1];
	int end_value=args[2];

	for(int i=0;i<5;i++){
		printf("%d=i\n",i);
		pthread_mutex_lock(&mutex[parity]);			
		printf("locked %d\n", parity);
		while(ready[parity]){
			if(i==5){
				pthread_mutex_unlock(&mutex[parity]);
				return NULL;
			}
			pthread_cond_wait(&cond[parity], &mutex[parity]);
		}
		for(int i=parity; i<10; i+=2){
			buff[i]=start_value + rand()%(end_value-start_value);
			printf("genearated %d at %d\n", buff[i], i);
		}	
		ready[parity]=1;
		pthread_cond_signal(&cond[parity]);
		pthread_mutex_unlock(&mutex[parity]);			
	}

	printf("producer %d done\n",parity);
	return NULL;
}

void main(){
	ready[0]=0;
	ready[1]=0;
	srand(time(NULL));
	pthread_mutex_init(&mutex[0], NULL);
	pthread_mutex_init(&mutex[1], NULL);
	pthread_cond_init(&cond[0], NULL);
	pthread_cond_init(&cond[1], NULL);

	int args_even[3] = {0, 0, 299};
	pthread_create(&t_even, NULL, generate, &args_even); 
	int args_odd[3] = {1, 300, 499};
	pthread_create(&t_odd, NULL, generate, &args_odd); 
	
	for(int i=0;i<5;i++){
		printf("trying to get locks...\n");
		pthread_mutex_lock(&mutex[0]);			
		while(!ready[0]){
			pthread_cond_wait(&cond[0], &mutex[0]);
		}
		printf("got lock0\n");

		pthread_mutex_lock(&mutex[1]);			
		while(!ready[1]){
			pthread_cond_wait(&cond[1], &mutex[1]);
		}
		printf("got lock1\n");
		int sum=0;
		for(int i=0;i<10;i++){
			sum+=buff[i];
		}
		if(sum>2000){
			printf("Zbir je veci od 2000\n");
		}
		
		ready[1]=0;
		pthread_cond_signal(&cond[1]);
		pthread_mutex_unlock(&mutex[1]);			

		ready[0]=0;
		pthread_cond_signal(&cond[0]);
		pthread_mutex_unlock(&mutex[0]);			
	}	
	printf("consumer done\n");

	pthread_join(t_even, NULL);
	pthread_join(t_odd, NULL);
	pthread_cond_destroy(&cond[0]);
	pthread_mutex_destroy(&mutex[0]);
	pthread_cond_destroy(&cond[1]);
	pthread_mutex_destroy(&mutex[1]);
}
