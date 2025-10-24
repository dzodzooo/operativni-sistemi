#include "include.h"
#include <sys/msg.h>
#include <stdio.h>

void main(){
	int msgid = msgget(MSGKEY, 0666);

	while(1){
		msg_t msg;
		msgrcv(msgid, &msg, sizeof(int), 0, 0);

		printf("TIP PORUKE JE %ld\n", msg.m_type);
		if(msg.m_type==2)
			break;

		printf("%d\n",msg.number);
	
	}
	printf("DRUGI KRAJ\n");
}
