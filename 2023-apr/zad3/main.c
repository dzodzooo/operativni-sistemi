#include <sys/msg.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define MSGKEY 10107
int msgid;
struct msgbuf{
	long mtype;
	int number;
};

void main(){
	msgid=msgget(MSGKEY, IPC_CREAT|0666);	
	int pid = fork();
	if(pid==-1){printf("error creating process\n"); exit(0);}
	if(pid==0){
		//process child
		int counter=0;	
		while(1){
			struct msgbuf msg;
			msgrcv(msgid, &msg, sizeof(int), 1, 0);
			if(msg.number==0){
				printf("Received %d messages \n",counter);
				break;
			}
			printf("Received %c\n",(char)msg.number);
			counter++;
		
		}

	
	}
	else
	{
		//process parent
		int number;
		while(1){
			struct msgbuf msg;
			msg.mtype=1;
			printf("Uneti broj[0-127]:\n");
			scanf("%d", &number);
			if(number<0 || number>127) continue;
			msg.number=number;
			msgsnd(msgid, &msg, sizeof(number), 0);
			if(number==0){
				
				break;	
			}
		}


		wait(NULL);
		msgctl(msgid, IPC_RMID, NULL);
	
	}

}
