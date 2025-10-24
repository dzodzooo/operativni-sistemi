#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

void main(int argc, char** argv){
	if(argc!=3){
		printf("usage ./main inputname outputname\n");
		return;
	}

	char* input=argv[1];
	char* output=argv[2];

	int fd[2];
	pipe(fd);
	int pid=fork();
	if(pid==0){
		close(fd[1]);
		FILE* f = fopen(output, "w");
		int line=0;
		while(1){
			unsigned char buff[1000];
			read(fd[0], &buff, 1000);
			if(strcmp(buff, "kraj")==0) break;
			int size=strlen(buff);
			char *xored=malloc(size);
			for(int i=0;i<size;i++, line++){
				xored[i]=(buff[i] ^ line);
			}
			fputs(xored, f);
			fputs(buff, f);
		}

		close(fd[0]);
		fclose(f);
	}
	else{
		close(fd[0]);
		unsigned char buff[1000];
		FILE* f = fopen(input, "r");
		while(fgets(buff, 1000, f)){
			write(fd[1], buff, 1000);
			memset(buff, '\0', 1000);
		}

		strcpy(buff, "kraj");
		write(fd[1], buff, 1000);
		close(fd[1]);
		fclose(f);
	}
}
