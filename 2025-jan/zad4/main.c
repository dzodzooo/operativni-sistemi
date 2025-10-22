#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

typedef struct {
	long int size;
	char* filepath;
} file_t;

file_t* max_file_size(file_t* file_max, file_t* file){
	if(!file_max)
		return file;
	
	if(file && file->size > file_max->size)
		return file;

	return file_max;
}

bool is_valid_for_search(char* filename, int level){
	return !(level==0 || strcmp(filename,".")==0 || strcmp(filename,"..")==0); 
}

char* get_path_from_filename(char* dir_path, char* filename){
	int len = strlen(dir_path)+strlen(filename)+2;
	char* fullpath=malloc(len);
	strcpy(fullpath, dir_path);
	strcat(fullpath, "/");
	strcat(fullpath, filename);
	return fullpath;
}

file_t* create_file_t(char* filepath, struct dirent* current){
	struct stat statbuf;
	stat(filepath, &statbuf);
	file_t* tmp=malloc(sizeof(file_t));
	tmp->size=statbuf.st_size;
	tmp->filepath=current->d_name;
	return tmp;
}



file_t* process_dir(char* dir_path, char* filename, int level){
	DIR* dirp = opendir(dir_path);
	if(!dirp){
		fprintf(stderr,"Couldn't read directory %s. errno=%d\n",dir_path,errno);
		exit(EXIT_FAILURE);
	}
	
	file_t* largest_file=NULL;
	struct dirent* current_file;
	while(current_file=readdir(dirp)) {
		struct stat statbuf;
		char* fullpath =get_path_from_filename(dir_path, current_file->d_name);
		if(current_file->d_type == DT_DIR)
		{
			if(!is_valid_for_search(current_file->d_name, level)) continue;

			file_t* tmp = process_dir(fullpath, current_file->d_name, level-1);
			largest_file = 	max_file_size(largest_file, tmp);	
			continue;
		}
		if(current_file->d_type == DT_REG){
			file_t* tmp = create_file_t(fullpath, current_file);
			printf("%s %ld\n",tmp->filepath, tmp->size);
			largest_file = max_file_size(largest_file, tmp);	
			continue;
		}
	}
	closedir(dirp);
	return largest_file;
}

file_t* find_largest_file(char* dir_path){
	int max_level=3;
	file_t* f = process_dir(dir_path,"", max_level);		
	return f;
}

void main(int argc, char** argv){
	if(argc!=2){
		fprintf(stderr,"Usage: ./main absolute-dir-path\n");
		exit(EXIT_FAILURE);
	}
	char* dir_path = argv[1];
	file_t* file = find_largest_file(dir_path);
	if(!file){
		fprintf(stderr, "Couldn't read any files.\n");
		exit(EXIT_FAILURE);
	}
	printf("Largest file in %s is %s, its size is %ld bytes.\n",dir_path,file->filepath, file->size);
}
