#include <stdlib.h>
#include <ctype.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>

#define MSGKEY 10106
#define MAX_SIZE 256

typedef struct msg_t{
	long int m_type;
	unsigned char line[MAX_SIZE];
} msg_t;

int msgid;

void main(){

	msgid=msgget(MSGKEY, IPC_CREAT|0666);
	if(msgid==-1){
		perror("msgget failed");
		exit(0);
	
	}
	int pid1, pid2;
	pid1 = fork();
	if(pid1==0){
		//child1
		FILE* f = fopen("druga.txt", "r");
		char buff[MAX_SIZE];
		while(fgets(buff, MAX_SIZE, f)){
			msg_t msg;
			msg.m_type=2;
			strcpy(msg.line, buff);
			strcat(msg.line,"\0");
			printf("saljem %s\n",msg.line);
			msgsnd(msgid,&msg,MAX_SIZE, 0);
			memset(buff,'\0',MAX_SIZE);
		}
		fclose(f);
		msg_t msg;
		msg.m_type=2;
		strcpy(msg.line, "kraj");	
		strcat(msg.line, "\0");
		printf("saljem %s\n",msg.line);
		msgsnd(msgid,&msg,MAX_SIZE,0);
	}
	else{
		pid2=fork();
		if(pid2==0){
			//child2
			FILE* f = fopen("prva.txt", "r");
			char buff[MAX_SIZE];
			while(fgets(buff, MAX_SIZE, f)){
				msg_t msg;
				msg.m_type=1;
				strcpy(msg.line, buff);
				printf("saljem %s\n",msg.line);
				msgsnd(msgid,&msg,MAX_SIZE,0);
				memset(buff,'\0',MAX_SIZE);
			}
			fclose(f);
			msg_t msg;
			msg.m_type=1;
			strcpy(msg.line, "kraj");	
			strcat(msg.line, "\0");
			printf("saljem %s\n",msg.line);
			msgsnd(msgid,&msg,MAX_SIZE,0);
		}
		else
		{
			//parent
			FILE* f1 = fopen("prva_mod.txt","w");
			FILE* f2 = fopen("druga_mod.txt","w");
			int kraj=0, size;
			while(1){
				msg_t msg;
				msgrcv(msgid, &msg, MAX_SIZE, 0, 0); 
				int mtype=msg.m_type;
				if(strcmp(msg.line, "kraj")==0)
				{
					kraj++;
					if(kraj==2) break;
					continue;
				}
				printf("kraj=%d primam %s", kraj, msg.line);	
				if(mtype==1)
				{
					for(int i=0;i<strlen(msg.line);i++){
						msg.line[i]=toupper(msg.line[i]);
					}

					fprintf(f1,"%s", msg.line);
				}
				else
					if(mtype==2)
					{
						for(int i=0;i<strlen(msg.line);i++){
							msg.line[i]=tolower(msg.line[i]);
						}
						fprintf(f2,"%s", msg.line);
					}
			}
			wait(NULL);
			wait(NULL);
			fclose(f1);
			fclose(f2);
			msgctl(msgid, IPC_RMID, NULL);
		}
	}
}
