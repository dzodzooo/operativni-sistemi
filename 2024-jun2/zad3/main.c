#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>
#include <sys/msg.h>

#define MSQKEY 10101

bool je_trocifren(int number){
	return number/100>0;
}

struct msgbuf {
	long mtype;
	int number;
};

void main(int argc, char** argv){
	int msqid=msgget(MSQKEY,IPC_CREAT|0666);
	int pid = fork();
	if(pid){
		int number = -1;
		struct msgbuf msg;
		while(number!=0){
			printf("Unesi broj: ");
			scanf("%d",&number);
			printf("\n");
			if(!je_trocifren(number)) 
				continue;
			msg.mtype=1;
			msg.number=number;
			msgsnd(msqid,&msg,sizeof(number),0);	
		}		
		msgsnd(msqid,&msg,sizeof(number),0);	
	}
	else
	{
		int number=-1,sum;
		struct msgbuf msg;

		while(number!=0){
			msgrcv(msqid,&msg,sizeof(number),1,0);
			number=msg.number;
			printf("received %d\n",number);
			sum=number%10+number/10%10+number/100;
			printf("Suma cifara broja %d je %d\n",number,sum);
		}	
	}

}
