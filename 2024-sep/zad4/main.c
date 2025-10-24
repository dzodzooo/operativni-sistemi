#include <sys/shm.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define SHMKEY 10105
#define SEMKEY 10108
#define MAGIC_NUMBER 5

int shmid, semid;
int* mem;

union semun {unsigned short* array;};
void main(){
	
	shmid = shmget(SHMKEY, (1+MAGIC_NUMBER) * sizeof(int), IPC_CREAT | 0666);
	if(shmid==-1) {return;}
	semid = semget(SEMKEY, 3, IPC_CREAT|0666);
	if(semid==-1) {return;}
	union semun v; 
	unsigned short values[3] = {1, 0, 1};
	v.array=values;
	if(-1==semctl(semid, 0, SETALL, v)){
		perror("couldnt SETALL");
		return;	
	} 
	struct sembuf lock1 = {0, -1, 0}; //read user input
	struct sembuf lock2 = {1, -1, 0}; //sum
	struct sembuf lock3 = {2, -1, 0}; //mutex
	struct sembuf unlock1 = {0, 1, 0};
	struct sembuf unlock2 = {1, 1, 0};
	struct sembuf unlock3 = {2, 1, 0};
	int* mem = (int*) shmat(shmid, NULL, 0);
	if(mem==NULL){return;}

	int pid = fork();
	if(pid==0){
		//child process
		int sum=0;
		while(1){
			semop(semid, &lock2, 1);
			semop(semid, &lock3, 1);
			if(mem[MAGIC_NUMBER]==-1) break;
			printf("summing...\n");
			sum=0;
			for(int i=0;i<MAGIC_NUMBER;i++)
				sum+=mem[i];
			mem[0]=sum;
			semop(semid, &unlock3, 1);
			semop(semid, &unlock1, 1);
		}
		
	}
	else{
		//parent process
		mem[MAGIC_NUMBER]=0;
		while(1){
			semop(semid, &lock1, 1);
			printf("locked lock1\n");
			semop(semid, &lock3, 1);
			printf("locked lock3\n");
			printf("Unesite %d brojeva u sledecem redu\n", MAGIC_NUMBER);
			for(int i=0;i<MAGIC_NUMBER;i++)
				scanf("%d",&mem[i]);
		
			semop(semid, &unlock3, 1);
			semop(semid, &unlock2, 1);

			semop(semid, &lock1, 1);
			semop(semid, &lock3, 1);
			int sum=mem[0];
			if(sum==0)
			{
				mem[MAGIC_NUMBER]=-1;
				break;
			}
			printf("Vasa suma je %d\n", sum);
			semop(semid, &unlock3, 1);
			semop(semid, &unlock1, 1);

		}
		kill(pid, SIGINT);
		shmctl(shmid, IPC_RMID, NULL);
		semctl(semid, 0, IPC_RMID);
		semctl(semid, 1, IPC_RMID);
		semctl(semid, 2, IPC_RMID);

	}
}
