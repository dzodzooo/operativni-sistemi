#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define MAX_LINE_SIZE 256
#define MAX_LINE_ELEMENTS_COUNT 128


typedef struct arr_t{
	int *elements;
	size_t size;
} arr_t;

pthread_mutex_t mutex;
pthread_cond_t can_sum;
pthread_cond_t sum_is_done;
int sum;
arr_t* arr;


pthread_cond_t can_exit_cond;
pthread_mutex_t can_exit;
int i_can_exit=0;


void free_arr_elements(arr_t* arr){
	free(arr->elements);
}
void reset_arr(arr_t* arr){
	for(size_t i=0;i<arr->size;i++){
		arr->elements[i]=0;
	}
	arr->size=0;
}

void parse_line(char* line, arr_t* arr){
	reset_arr(arr);
	char* element=strtok(line," ");
	size_t i=0;
	arr->elements[i++]=atoi(element);
	printf("%d ",arr->elements[i-1]);
	while((element=strtok(NULL," "))){
		arr->elements[i++]=atoi(element);
		printf("%d ",arr->elements[i-1]);
	}	
	printf("\n");
	arr->size=i;
}

void sum_arr(void *args){
	do{
		pthread_mutex_lock(&mutex);
		//printf("Side thread locked mutex.\n");
		while(sum>=0)
		{
			//printf("waiting...\n");
			pthread_cond_wait(&can_sum,&mutex);
		}
		//printf("summing...\n");
		arr_t* arr = (arr_t*)args;
		int s=0;
		for(size_t i=0;i<arr->size;i++){
			s+=arr->elements[i];
		}
		sum=s;
		pthread_cond_signal(&sum_is_done);
		pthread_mutex_unlock(&mutex);
		//printf("Side thread unlocked mutex.\n");
		
		pthread_mutex_lock(&can_exit);
		//printf("checking if i can exit\n");
		if(i_can_exit) break;
		pthread_mutex_unlock(&can_exit);
		
		//printf("Side thread cannot exit.\n");
	}
	while(1);
	//printf("finished\n");
}

int main(int argc, char** argv){

	if(argc!=2){
		fprintf(stderr,"Program usage: ./zad1 filename \n");
		exit(EXIT_FAILURE);
	}
	char* filename= argv[1];
	FILE* f = fopen(filename, "r");
	if(!f) {
		fprintf(stderr,"Couldn't read file %s.\n",filename);
	}

	arr=malloc(sizeof(arr_t));
	arr->elements=malloc(sizeof(int)*MAX_LINE_ELEMENTS_COUNT);

	pthread_mutex_init(&mutex,NULL);
	pthread_mutex_init(&can_exit,NULL);
	pthread_cond_init(&can_sum,NULL);
	pthread_cond_init(&can_exit_cond,NULL);
	pthread_cond_init(&sum_is_done,NULL);
	
	pthread_t tid;
	pthread_create(&tid, NULL, (void*)sum_arr, (void*)arr);

	char buff[MAX_LINE_SIZE];
	int counter=0;
	while(fgets(buff,MAX_LINE_SIZE,f)){
		pthread_mutex_lock(&mutex);
		//printf("Main locked mutex.\n");
		parse_line(buff, arr);
		sum=-1;
		//printf("Side thread, you can sum.\n");
		pthread_cond_signal(&can_sum);
		while(sum==-1)
			pthread_cond_wait(&sum_is_done,&mutex);	
		printf("s%d=%d\n",counter++,sum);
		//printf("Main unlocked mutex.\n");
		pthread_mutex_unlock(&mutex);
	}

	fclose(f);


	//printf("Main locking can_exit mutex.\n");	
	pthread_mutex_lock(&mutex);
	pthread_mutex_lock(&can_exit);
	i_can_exit=1;
	sum=-1;
	//printf("Main says you can exit, side thread.\n");	
	pthread_cond_signal(&can_sum);
	pthread_mutex_unlock(&can_exit);
	pthread_mutex_unlock(&mutex);
	
	//printf("Main unlocked all locks.\n");
	
	pthread_join(tid, NULL);
	//printf("Threads joined");	
	
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&can_sum);
	pthread_cond_destroy(&sum_is_done);
	
	reset_arr(arr);
	free(arr);
	
	return 0;
}
