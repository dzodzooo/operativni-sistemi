#include <unistd.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include "include.h"
void main(){
	int msgid = msgget(MSGKEY, IPC_CREAT | 0666);
	int pid1=fork();
	if(pid1==0){
		//child process
		execl("./prvi", "./prvi", (char*) NULL);
	}
	int pid2=fork();
	if(pid2==0){
		//child process
		execl("./drugi", "./drugi", (char*) NULL);
	}
	wait(NULL);
	wait(NULL);
	msgctl(msgid, IPC_RMID, NULL);
}
