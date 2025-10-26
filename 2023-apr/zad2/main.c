
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void main(int argc, char** argv){
	if(argc!=3){printf("usage ./main prog arg\n"); exit(0);}

	char* prog=argv[1];
	char* arg=argv[2];
	int pid=fork();
	if(pid==0){
		execlp(prog, prog, arg, (char*) NULL);
	}
	int status;
	wait(&status); 
	printf("Program %s %s executed with code %d\n", prog, arg, status);
}
