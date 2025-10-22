#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <stdlib.h>
#define MSGKEY 10105

typedef struct student_t{
	int indeks;
	char ime[20], prezime[30];
} student_t;

struct msgbuf{
	long mtype;
	student_t student;
};

int msgid;

void main(){
	msgid=msgget(MSGKEY, IPC_CREAT | 0666);
	if(msgid==-1){
	
		perror("msgget error");
		exit(0);
	}


	int pid1, pid2;
	if((pid1=fork())==-1)
	{
		perror("fork1 bad");
		exit(0);
	}
	if(pid1==0){
		//child1
		struct msgbuf msg;
		FILE* f=fopen("stara_akreditacija.txt", "w");
		if(!f){
			perror("cant open stara_akreditacija.txt");
		}
		while(1){
			if(-1==msgrcv(msgid, &msg, sizeof(student_t), 1, 0)){
				perror("msgrcv error");
			}	
			student_t s = msg.student;
			if(s.indeks==10000)
			{
				break;
			}
			fprintf(f, "%d %s %s\n", s.indeks, s.ime, s.prezime);
		}
		fclose(f);

	}
	else
	{
		//parent
		if((pid2=fork())==-1)
		{
			perror("fork2 bad");
			exit(0);
		}
		if(pid2==0){
			//child2	
			struct msgbuf msg;
			FILE* f=fopen("nova_akreditacija.txt", "w");
			if(!f){
				perror("cant open nova_akreditacija.txt");
			}
			while(1){
				if(-1==msgrcv(msgid, &msg, sizeof(student_t), 2, 0)){
					perror("msgrcv error");
				}	
				student_t s = msg.student;
				if(s.indeks==10000)
				{
					break;
				}
				fprintf(f, "%d %s %s\n", s.indeks, s.ime, s.prezime);
			}
			fclose(f);
		}
		else{
			//parent
			struct msgbuf msg;
			while(1){
				student_t s;
				printf("Uneti broj indeksa, ime i prezime studenta\n");
				scanf("%d %s %s", &s.indeks, s.ime, s.prezime);
				msg.student=s;
				if(s.indeks==10000){
					msg.mtype=1;
					msgsnd(msgid, &msg, sizeof(student_t),0); 
					msg.mtype=2;
					msgsnd(msgid, &msg, sizeof(student_t),0); 
					break;
				}
				if(s.indeks<17000){
					//stara akreditacija
					msg.mtype=1;
				}
				else{
					//nova akreditacija
					msg.mtype=2;
				}
				if(-1==msgsnd(msgid, &msg, sizeof(student_t),0)){
					perror("msgsnd error\n");
				} 
			}
			wait(NULL);
			wait(NULL);
			msgctl(msgid, IPC_RMID, NULL);
		}

	}
}

