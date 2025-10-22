#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
void main(int argc, char** argv){

	if(argc!=3)
	{
		fprintf(stderr,"Usage: ./main to-be-executed");
		exit(EXIT_FAILURE);
	}
	char* to_be_executed=argv[1];
	const char* arguments=argv[2];
	int pid=fork();
	if(pid==0){
		execl(to_be_executed, to_be_executed, arguments, NULL);
	}
	else{
		int wstatus;
		wait(&wstatus);
		if(WIFEXITED(wstatus))
			printf("%d\n",WEXITSTATUS(wstatus));
	}
}
