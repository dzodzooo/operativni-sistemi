#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/errno.h>
#define MAX_SENTENCE_SIZE 256

int main2child[2], child2main[2];

void main_process_do(char* sentence){
	sentence=strtok(sentence,"\n");
	printf("Obradjujemo recenicu: \"%s\"\n",sentence);
	char* word=strtok(sentence," ");
	do{
		printf("Evo ti rec %s\n",word);
		write(main2child[1], word, strlen(word)+1);
		char sum[64];	
		read(child2main[0],sum,64);
		printf("Suma cifara u reci %s je %s\n",word,sum);
		word=strtok(NULL," ");
	}
	while(word!=NULL);
	printf("Nema vise reci.\n");
	write(main2child[1],"end\0",4);
}

int sum_word_digits(char* word){
	int sum=0;
	int len=strlen(word);
	for(int i=0; i<len; i++){
		if(word[i]>='0'&&word[i]<='9')
			sum+=word[i]-'0';
	}
	return sum;	
}

void child_process_do(){
	while(1){
		char word[MAX_SENTENCE_SIZE];
		printf("Ocekujem novu rec.\n");
		read(main2child[0],word,MAX_SENTENCE_SIZE);
		printf("Primam rec %s\n",word);
		if(strcmp(word,"end")==0) break;
		int sum=sum_word_digits(word);
		char sum_s[64];
		sprintf(sum_s,"%d",sum);
		write(child2main[1],sum_s,strlen(sum_s)+1);
	}
	close(main2child[0]);
	close(child2main[1]);
}

int main(int argc, char** argv){
	char sentence[MAX_SENTENCE_SIZE];
	printf("Unesi recenicu.\n");
	fgets(sentence, MAX_SENTENCE_SIZE,stdin);

	if(pipe(main2child)||pipe(child2main)){
		fprintf(stderr,"Neuspesno pipe-ovanje. errno:%d\n",errno);
		exit(EXIT_FAILURE);		
	}

	int pid=fork();
	if(pid==0){
		close(main2child[1]);
		close(child2main[0]);
		child_process_do();
	}
	else
	{
		close(main2child[0]);
		close(child2main[1]);
		main_process_do(sentence);
		wait(NULL);
		close(main2child[1]);
		close(child2main[0]);
	}
	
	return 0;
}
