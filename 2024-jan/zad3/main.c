#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>

#define LINE_MAX_SIZE 64
#define MSGQ_KEY 10101

int MSGQ_ID;

typedef struct {
	char ime[15];
	char prezime[20];
	int  broj_indeksa;

} student_t;

typedef struct {
	long  type;
	student_t student;
	char msg[20];
} message_t;

void child_process_do(int type){
	while(1){
		message_t m;
		int m_size = sizeof(message_t)-sizeof(long);
		msgrcv(MSGQ_ID, &m, m_size, type, 0);
		if(strcmp(m.msg,"kraj")==0){
			break;
		}
		student_t s = m.student;
		printf("Amfiteatar %d - %s %s %d\n",type,s.ime,s.prezime,s.broj_indeksa);
	}
	printf("%d done.",type);
}

student_t parse_line(char* line){
	student_t s;
	char* ime = strtok(line," ");
	strcpy(s.ime,ime);
	char* prezime=strtok(NULL," ");
	strcpy(s.prezime,prezime);
	s.broj_indeksa=atoi(strtok(NULL," "));
	return s;
}

void send2child(student_t s, long type, char* message){
	message_t msg;
	msg.type=type;
	msg.student=s;
	strcpy(msg.msg,message);
	int msg_size=sizeof(message_t)-sizeof(long);
	//printf("Main sending you(%ld) a msg, %d\n",msg.type,s.broj_indeksa);
	msgsnd(MSGQ_ID, &msg, msg_size, 0);
}

void main_process_do(){
	FILE* f = fopen("prijava.txt","r");
	if(!f){
		fprintf(stderr,"File doesn't exist.\n");
		exit(EXIT_FAILURE);
	}
	char line[LINE_MAX_SIZE];
	while(fgets(line, LINE_MAX_SIZE,f)){
		student_t s = parse_line(line);
		send2child(s, s.broj_indeksa>18000?1:2, "student");
	}
	student_t s;
	send2child(s, 1, "kraj");
	send2child(s, 2, "kraj");
}



void main(int argc, char** argv){
	MSGQ_ID=msgget(MSGQ_KEY, IPC_CREAT|0666);
	int pid1 = fork();
       	if(pid1==0){
       		//amfi 1
       		child_process_do(1);
       	}	
       	else{
       		int pid2=fork();
       		if(pid2==0){
			//amfi 2
			child_process_do(2);
		}
		else{
			//main process
			main_process_do();
			waitpid(pid1,NULL,0);
			waitpid(pid2,NULL,0);
			msgctl(MSGQ_ID, IPC_RMID, NULL);
			printf("Main done.\n");
		}
       	}
}
