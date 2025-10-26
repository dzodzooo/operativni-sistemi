#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdbool.h>

char* construct_filepath(char* dirname, char* filename){
	char* filepath=malloc(strlen(dirname)+strlen(filename)+2);
	strcpy(filepath, dirname);
	strcat(filepath, "/");
	strcat(filepath, filename);
	strcat(filepath, "\0");
	return filepath;
} 
typedef struct file_t {
	char* name;
	int size;
} file_t;

file_t search(char* dirname, int lvl){
	file_t max_reg;
	max_reg.size=0;

	DIR* d = opendir(dirname);
	if(!d){printf("cant read dir %s\n",dirname);exit(EXIT_FAILURE);}
	struct dirent* file;
       	while(file=readdir(d)){
		if(strcmp(file->d_name,".")==0 || strcmp(file->d_name,"..")==0) continue;
		char* filepath = construct_filepath(dirname, file->d_name);
		struct stat stats;
		printf("%s is filepath\n", filepath);
		stat(filepath, &stats);
		if(S_ISDIR(stats.st_mode) && lvl > 0){
			file_t nested=search(filepath, lvl-1);
			if(nested.size>max_reg.size){
				max_reg.size=nested.size;
				max_reg.name=nested.name;
			}
			continue;
		}
		if(S_ISREG(stats.st_mode)){
			printf("found regular file %s\n", file->d_name);
			if(max_reg.size==0 || stats.st_size>max_reg.size){
				max_reg.size=stats.st_size;
				max_reg.name=file->d_name;
			}
		}
	}
	closedir(d);
	return max_reg;
}

void main(int argc, char** argv){
	if(argc!=2){
		printf("usage ./main dirname\n");
		exit(0);	
	}
	char* dirname=argv[1];
	file_t max_reg=search(dirname, 5);
	printf("MAX REG FILE IS %s\n", max_reg.name);
}
