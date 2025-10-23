#include <dirent.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void search(char* filepath){

	DIR* d = opendir(filepath);
	if(!d){
		printf("couldnt open directory %s\n",filepath);
		exit(0);
	}
	struct dirent* info;
	while(info=readdir(d)){
		if(strcmp(info->d_name,".")==0||strcmp(info->d_name,"..")==0)continue;
		char *newpath = malloc(strlen(filepath)+strlen(info->d_name)+2);
		strcpy(newpath, filepath);
		strcat(newpath, "/");
		strcat(newpath, info->d_name);
		strcat(newpath, "\0");
		struct stat *stats = malloc(sizeof(struct stat));
		stat(newpath, stats);

		if(S_ISDIR(stats->st_mode)){
			//printf("found dir %s\n",newpath);
			search(newpath);	
		}	
				
		else if(S_ISREG(stats->st_mode)){
			//printf("found file %s\n",newpath);
			if(strstr(info->d_name,".sh")){
				printf("trying to execute file %s\n",newpath);
					int pid=fork();
					if(pid==-1) {
						printf("Couldn't fork\n");
					}
					else 
						if(pid==0) 			{
							execlp("sh", "sh", newpath, (char*)NULL);
						}
						else
						{
							wait(NULL);
						}
			}
		}
	}
	closedir(d);
}

void main(int argc, char** argv){
	if(argc!=2)
	{
		printf("usage ./a.out filepath\n");
		exit(0);
	}
	char* filepath = argv[1];
	printf("reading directory %s\n",filepath);
	search(filepath);
}
