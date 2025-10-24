#include "include.h"

void main(){
	printf("running drugi...\n");
	int semid=semget(SEMKEY, 3, 0666);
 	int msgid=msgget(MSGKEY, 0666);
	if(msgid==-1){
		printf("couldnt get msgid\n");
	
	}
	FILE* f = fopen("sredjeno.txt", "a");
	if(!f){
		printf("error opening sredjeno.txt\n");
		return;
	}
	while(1){
		struct msgbuf msg;

		if(msgrcv(msgid, &msg, MAX_SIZE, 2, 0)==-1)
			printf("error receiving\n");

		if(strcmp(msg.line, "kraj")==0){
			break;
		}
		semop(semid, &lock2, 1);
		semop(semid, &lock3, 1);
		fprintf(f, "%ld %s",msg.mtype, msg.line);
		fflush(f);
		semop(semid, &unlock3, 1);
		semop(semid, &unlock1, 1);
	}
	fclose(f);
	printf("drugi out\n");
}
