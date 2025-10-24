#include "include.h"

void main(){
 	int msgid=msgget(MSGKEY, IPC_CREAT | 0666);
	int semid=semget(SEMKEY, 3, IPC_CREAT|0666);
	union semun arg;
	unsigned short array[3] = {1, 0, 1};
	arg.array=array;
	semctl(semid, 0, SETALL, arg);
	if(msgid==-1)
	{
		printf("error getting msgid\n");
		return;
	}

	int pid1=fork();
	if(pid1==0)
		execl("prvi", "prvi",(char*)NULL);

	int pid2=fork();
	if(pid2==0)
		execl("drugi", "drugi",(char*)NULL);

	FILE* f = fopen("poruka.txt", "r");
	if(!f){
		printf("no such file poruka.txt\n");
		return;
	}
	int line=1;
	char buff[MAX_SIZE];
	while(fgets(buff,MAX_SIZE,f))
	{
		struct msgbuf msg;
		strcpy(msg.line, buff);
		if(line%2==0)
		{
			msg.mtype=2;
		}
		else
		{
			msg.mtype=1;
		}
		msgsnd(msgid, &msg, MAX_SIZE, 0);
		printf("Sent %s mtype=%ld\n",msg.line, msg.mtype);
		line++;
	}
	fclose(f);
	struct msgbuf msg;
	strcpy(msg.line, "kraj");
	msg.mtype=2;
	msgsnd(msgid, &msg, MAX_SIZE, 0);
	msg.mtype=1;
	msgsnd(msgid, &msg, MAX_SIZE, 0);
	wait(NULL);
	wait(NULL);
	msgctl(msgid, IPC_RMID, NULL);
	semctl(semid, 0, IPC_RMID);
	semctl(semid, 1, IPC_RMID);
	semctl(semid, 2, IPC_RMID);
	printf("main process done\n");
}
