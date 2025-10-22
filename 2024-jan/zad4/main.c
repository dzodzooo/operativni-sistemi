#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <stdbool.h>

#define BUFF_MAX_SIZE 128

#define SHM_KEY 10101
#define SHM_SIZE 10

#define SEM_KEY 10101
#define SEM2_KEY 10102
#define SEM_NUM 1

/*
typedef struct sembuf{
	unsigned_short sem_num;	
	short sem_op;		
	short sem_flg;	
} sembuf_t;
*/
typedef union semun{
	int value;
} semun;

struct sembuf minus_one = {0, -1, 0};
struct sembuf plus_one = {0, 1, 0};

int shmid;
int sem_canpost_id, sem_canread_id;

int sum_array(int* array, int size){
	int sum=0;
	for(int i=0;i<size;i++){
		sum+=array[i];
	}
	return sum;
}

void consume(int *arr){
	printf("consuming\n");
	while(1){
		semop(sem_canread_id, &minus_one, 1);
		printf("printing array\n");
		int sum=sum_array(arr, 10);
		printf("sum=%d\n",sum);
		semop(sem_canpost_id, &plus_one, 1);		
	}
}

void cleanup(int* arr){
	shmdt(arr);
	semctl(sem_canpost_id, SEM_NUM, IPC_RMID);
	semctl(sem_canread_id, SEM_NUM, IPC_RMID);
	shmctl(shmid, IPC_RMID, NULL);
}

int read_lines(FILE* f, int arr[10]){
	char buff[BUFF_MAX_SIZE];
	int i=0;
	while(fgets(buff,buff[BUFF_MAX_SIZE],f))
	{
		int number = atoi(buff);
		arr[i++]=number;
		if(i==10)
			break;	
	}
	return i;		
}

int* get_shm(int size){
	int* arr =shmat(shmid, NULL, 0);
	return arr;	
}

FILE* open_file(char* filename)
{
	FILE* f = fopen(filename,"r");
	if(!f){
		fprintf(stderr,"Can't open file\n");
	}
	return f;
}

void produce(FILE* f, int* arr){
	printf("producing\n");
	bool is_end = false;
	while(1){
		semop(sem_canpost_id, &minus_one, 1);
		printf("reading lines\n");
		int lines_read=read_lines(f, arr);
		is_end=!(lines_read==10);
		if(is_end && lines_read==0)
		{
			break;	
		}
		else if(is_end && lines_read>0){
			for(int i=lines_read;i<10;i++){
				arr[i]=0;
			}
		}
		semop(sem_canread_id, &plus_one, 1);	
	}
	printf("finished reading\n");
	
}


void main(int argc, char** argv){
	int pid=fork();
	if(pid==0){
		sem_canpost_id = semget(SEM_KEY,  SEM_NUM, IPC_CREAT|0666);
		sem_canread_id = semget(SEM2_KEY, SEM_NUM, IPC_CREAT|0666);
		shmid 	       = shmget(SHM_KEY, SHM_SIZE*sizeof(int), IPC_CREAT|0666);
		if(!shmid){
			printf("error getting memory");
		}
		int* arr = get_shm(10);
		consume(arr);				
		cleanup(arr);
	}
	else{
		sem_canpost_id = semget(SEM_KEY,  SEM_NUM, IPC_CREAT|0666);
		sem_canread_id = semget(SEM2_KEY, SEM_NUM, IPC_CREAT|0666);
		shmid 	       = shmget(SHM_KEY, SHM_SIZE*sizeof(int), IPC_CREAT|0666);
	
		semun semopts;
		semopts.value=1;
		semctl(sem_canpost_id, 0, SETVAL, semopts);
		semopts.value=0;
		semctl(sem_canread_id, 0, SETVAL, semopts);

		FILE* f=open_file("brojevi.txt");
		if(!f)
		{
			kill(pid,SIGINT);
			exit(EXIT_FAILURE);
		}
		int* arr = get_shm(10);
		produce(f, arr);
		kill(pid,SIGINT);
		cleanup(arr);
		fclose(f);
	}
}
