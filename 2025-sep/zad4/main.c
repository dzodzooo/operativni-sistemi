#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

typedef struct file_t {
	char* name;
	long int size;
	long int inode;
} file_t;

char* create_fullpath(char* dirpath, char* filename){
	char* path = malloc(strlen(filename)+strlen(dirpath)+1+1);
	strcpy(path, dirpath);
	strcat(path, "/");
	strcat(path, filename);
	strcat(path, "\0");
	return path;
}

file_t find_1kb_txt(char* dirpath, int N){
	file_t  min;
	min.name=malloc(30);
	min.size=LONG_MAX;
	DIR* dir = opendir(dirpath);
	if(!dir) {fprintf(stderr,"cant open dir %s", dirpath); exit(EXIT_FAILURE);} 

	struct dirent* file;
	while(file=readdir(dir)){
		char* filename = file->d_name;
		if(strcmp(filename, ".")==0 || strcmp(filename, "..")==0){
			continue;
		}

		char* path = create_fullpath(dirpath, filename);
		struct stat stats;
		stat(path, &stats);

		if(S_ISDIR(stats.st_mode) && N>1){
			printf("%s is dir\n", path);	
			file_t nested_min = find_1kb_txt(path, N-1);		
			if(nested_min.size > 1024 && nested_min.size < min.size)
			{
				min.size=nested_min.size;
				min.inode=nested_min.inode;
				strcpy(min.name, nested_min.name);
			}
			continue;
		}
		if(S_ISREG(stats.st_mode)){
			printf("%s is regular file\n", path);
			if(min.size==0 || stats.st_size>1024 && stats.st_size<min.size)
			{
				min.size=stats.st_size;
				min.inode=stats.st_ino;
				strcpy(min.name, filename);
			}
		}
	}
	closedir(dir);
	return min;
}

void main(int argc, char** argv){
	if(argc!=2){
		perror("usage ./a.out full/path/to/dir");
		exit(0);
	}
	char* path = argv[1];

	int N;
	printf("uneti N\n");
	scanf("%d",&N);

	file_t min=find_1kb_txt(path, N);
	if(min.name!=NULL)
		printf("name:%s inode: %ld size:%ldB\n", min.name, min.inode, min.size);
}
