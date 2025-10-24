#include "include.h"

void main(){
	printf("running prvi...\n");
 	int msgid=msgget(MSGKEY, 0666);
	int semid=semget(SEMKEY, 3, 0666);
	FILE* f = fopen("sredjeno.txt", "a");
	if(!f){
		printf("error opening sredjeno.txt\n");
		return;
	}
	
	while(1){
		struct msgbuf msg;

		if(msgrcv(msgid, &msg, MAX_SIZE, 1, 0)==-1)
			printf("error receiving\n");

		if(strcmp(msg.line, "kraj")==0){
			break;
		}
		semop(semid, &lock1, 1);
		semop(semid, &lock3, 1);
		fprintf(f, "%ld %s",msg.mtype,msg.line);
		fflush(f);
		semop(semid, &unlock3, 1);
		semop(semid, &unlock2, 1);
	}
	fclose(f);
}
