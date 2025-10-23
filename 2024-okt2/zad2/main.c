#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define MAX_SIZE 256

void main(){
	int fd1[2], fd2[2];
	pipe(fd1);
	pipe(fd2);
	int pid = fork();
	if(pid==0){
		//child process	
		close(fd1[1]);
		close(fd2[0]);

		int size;
		read(fd1[0], &size, sizeof(size));
		char* path = malloc(size+1);
		strcat(path,"\0");
		read(fd1[0], path, size);
		printf("PATH = %s\n", path);

		read(fd1[0], &size, sizeof(size));
		char* keyword = malloc(size+1);
		read(fd1[0], keyword, size);
		close(fd1[0]);

		printf("KEYWORD = %s\n", keyword);

		FILE* file = fopen(path, "r");
		char buff[MAX_SIZE];
		int line=0;
		while(fgets(buff, MAX_SIZE,file))
		{
			if(strstr(buff, keyword)!=NULL){
				write(fd2[1], &line, sizeof(line));	
			}
			line++;
		}
		fclose(file);
		line=-1;
		write(fd2[1], &line, sizeof(line));	
		close(fd2[1]);
	}
	else
	{
		//parent process
		close(fd1[0]);
		close(fd2[1]);

		char path[30], keyword[30];
		printf("Enter path:\n");
		scanf("%s", path);
		printf("Enter keyword:\n");
		scanf("%s", keyword);
		printf("%s %s\n", path, keyword);
		int size = strlen(path);
		write(fd1[1], &size, sizeof(size));
		write(fd1[1], path, strlen(path));
		size = strlen(keyword);
		write(fd1[1], &size, sizeof(size));
		write(fd1[1], keyword, strlen(keyword));
		close(fd1[1]);
		
		int line;
		while(1){
			read(fd2[0], &line, sizeof(line));	
			if(line==-1) break;
			printf("%d\n",line);
		}

		close(fd2[0]);
		wait(NULL);
	}
}
