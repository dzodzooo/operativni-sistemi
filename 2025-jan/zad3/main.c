#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

#define MSQKEY 10101
#define SEMKEY 10102
struct msgbuf{
	long mtype;
	int number;
};

union semun {
	int val;
};
void main(int argc, char** argv){
	
	int pid=fork();
	int msqid=msgget(MSQKEY,0666|IPC_CREAT );
	int semid=semget(SEMKEY,1, 0666|IPC_CREAT );
	union semun arg;
	arg.val=1;
	semctl(semid,0, SETVAL, arg);
	struct sembuf lock = {0,-1,0};
	struct sembuf unlock = {0,1,0};
	if(pid==0){
		for(int i=0;i<10;i++){
			struct msgbuf msg;
			msgrcv(msqid, &msg, sizeof(int), 0,0);
			int number=msg.number;
			int sum=0;
			while(number>0){
				sum+=number%10;
				number/=10;
			}
			semop(semid, &lock, 1);
			printf("SUMA CIFARA BROJA %d je %d.\n",msg.number,sum);
			semop(semid, &unlock, 1);
		}
	}
	else
	{
		for(int i=0;i<10;i++){
			int number;
			semop(semid, &lock, 1);
			printf("Uneti visecifreni broj: ");
			scanf("%d",&number);
			semop(semid, &unlock, 1);
			printf("\n");
			struct msgbuf msg;
			msg.mtype=1;
			msg.number=number;
			msgsnd(msqid, &msg, sizeof(int),0);
		}
		wait(NULL);
		msgctl(msqid,IPC_RMID,NULL);
		semctl(semid,0,IPC_RMID,NULL);
	}
}
