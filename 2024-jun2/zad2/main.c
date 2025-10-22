#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>

int pipefd12[2], pipefd23[2], pipefd13[2];


void child2_do(){
	close(pipefd12[0]);
	close(pipefd12[1]);
	close(pipefd23[1]);
	close(pipefd13[0]);
	
	int line;
	int sum=0;
	while(1){
		read(pipefd23[0],&line,sizeof(line));
		if(line==-1)
			break;
		sum+=line;
	}	
	printf("Suma prosledjenih rednih brojeva linija: %d\n",sum);
	
	close(pipefd23[0]);
	close(pipefd13[1]);
}

void main_process_do(char* filename, int argc, char** argv){
	close(pipefd23[0]);
	close(pipefd23[1]);
	close(pipefd12[0]);
	close(pipefd13[1]);

	int strlength=strlen(filename);
	write(pipefd12[1],&strlength,sizeof(int));
	write(pipefd12[1],filename,strlength);
	int argcnt=argc-2;
	write(pipefd12[1],&argcnt,sizeof(int));
	for(int i=2;i<argc;i++){
		strlength=strlen(argv[i]);
		write(pipefd12[1],&strlength,sizeof(int));
		write(pipefd12[1],argv[i],strlength);
	}
	close(pipefd12[1]);
	close(pipefd13[0]);
	wait(NULL);
	wait(NULL);
}

bool contains_magic_words(char* line, char** words, int words_count){
	for(int i=0;i<words_count;i++){
		if(strstr(line,words[i]))
				return true;
	}
	return false;
}

void child1_do(){
	close(pipefd13[0]);
	close(pipefd13[1]);
	close(pipefd12[1]);
	close(pipefd23[0]);
	
	char filename[64];
	int argc, strlength;	
	read(pipefd12[0],&strlength,sizeof(int));
	read(pipefd12[0],filename,strlength);
	read(pipefd12[0],&argc,sizeof(int));
	char** args = malloc(sizeof(char*)*argc);
	for(int i=0;i<argc;i++){
		read(pipefd12[0],&strlength,sizeof(int));
		args[i]=malloc(strlength);
		read(pipefd12[0],args[i],strlength);
	}
	printf("Primio sam filename=%s\n",filename);
	printf("Primio sam argc=%d\n",argc);
	printf("Primio sam args=%s\n",args[0]);
	
	FILE* f = fopen(filename, "r");
	if(!f){
		fprintf(stderr,"error opening file %s\n",filename);
	}

	char buff[64];
	int line=0;
	while(fgets(buff,64,f)){
		if(contains_magic_words(buff, args, argc))
			write(pipefd23[1],&line,sizeof(line));
		line++;
	}
	line=-1;
	write(pipefd23[1],&line,sizeof(line));
	fclose(f);	
	close(pipefd12[0]);
	close(pipefd23[1]);
}

void main(int argc, char** argv){
	if(argc < 2){
		fprintf(stderr,"Usage: ./main filename args");
		exit(EXIT_FAILURE);
	}
	char* filename=argv[1];
	int argcnt=argc-2;
	int pid1, pid2;
	pipe(pipefd12);
	pipe(pipefd23);
	pipe(pipefd13);
	if(pid1=fork()){
		if(pid2=fork()){
			main_process_do(filename, argc, argv);
		}
		else
		{
			//child2, 3
			child2_do();
		}
	}
	else
	{
		//child1, 2
		child1_do();
	}

}
