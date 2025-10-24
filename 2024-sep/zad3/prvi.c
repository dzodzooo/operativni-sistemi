#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/msg.h>
#include "include.h"

void main(){
	int msgid = msgget(MSGKEY, 0666);
	int number, sum=0;
	while(1){
		printf("Uneti broj:\n");
		scanf("%d", &number);
		sum+=number;
		msg_t msg;
		msg.number=number;
		if(sum>1000) 
		{
			msg.m_type=2;
			msgsnd(msgid, &msg, sizeof(number), 0);
			break;
		}
		msg.m_type=1;
		msgsnd(msgid, &msg, sizeof(number), 0);
	}
	printf("PRVI KRAJ\n");
}
