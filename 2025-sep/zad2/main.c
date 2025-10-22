#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

void main(){
	int fd[2];
	if(pipe(fd)==-1){
		perror("pipe bad");
		exit(0);
	}
	int pid=fork();
	if(pid==-1){
		perror("fork bad");
		exit(0);
	}
	if(pid==0){
		//child process	
		close(fd[1]);	
		int number;
		while(1){
			read(fd[0], &number, sizeof(int));
			if(number%2==0){
			
				printf("%d\n",number);
			}
			if(number==-1){
				break;
			}
		}
		close(fd[0]);
	}
	else{
		//parent process
		close(fd[0]);
		srand(time(NULL));
		int number, counter=0;
		while(1){
			number = rand()%99;
			write(fd[1], &number, sizeof(number));
			if(number%2==0)
			{
				counter=0;
			}
			else counter++;
			if(counter==3){
				break;	
			}
		}
		number=-1;
		write(fd[1], &number, sizeof(number));
		close(fd[1]);
		wait(NULL);
	}
	
}
