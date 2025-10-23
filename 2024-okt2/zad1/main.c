#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#define MAX_SIZE 256

pthread_t t1, t2;
pthread_mutex_t mutex;
pthread_cond_t cond;

FILE* outputf;
int line;

typedef struct msg_t{
	char* filename;
	int condition;
} msg_t;

void* routine (void* args){
	msg_t* msg = (msg_t*) args;
	char* filename = msg->filename;
	printf("filename %s\n",filename);
	int condition = msg->condition;

	FILE* f = fopen(filename, "r");
	char buff[MAX_SIZE];
	int counter=0;
	while(fgets(buff, MAX_SIZE, f)){
		counter++;
		pthread_mutex_lock(&mutex);
		while(line%2!=condition){
			pthread_cond_wait(&cond, &mutex);	
		}
		fputs(buff, outputf);
		line++;
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
		memset(buff,'\0',MAX_SIZE);

	}
	fclose(f);

	return NULL;
}
void main (int argc, char** argv){

	if(argc!=3){
		perror("usage ./a.out filename1 filename2");
		exit(0);
	}

	char* filename1 = argv[1];
	char* filename2 = argv[2];
	line=0;
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	outputf=fopen("output.txt", "w");
	msg_t msg1;
	msg1.filename=filename1;
	msg1.condition=0;
	msg_t msg2;
	msg2.filename=filename2;
	msg2.condition=1;
	pthread_create(&t1, NULL, routine, &msg1);
	pthread_create(&t2, NULL, routine, &msg2);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
	fclose(outputf);
}
