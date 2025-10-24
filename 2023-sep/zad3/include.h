#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <string.h>

#define MAX_SIZE 256
#define MSGKEY 10109
#define SEMKEY 10110

union semun {
	int val;
	struct semid_ds *buf;
	unsigned short  *array;
	struct seminfo  *__buf;
};

struct sembuf lock1={0,-1,0};
struct sembuf lock2={1,-1,0};
struct sembuf lock3={2,-1,0};
struct sembuf unlock1={0,1,0};
struct sembuf unlock2={1,1,0};
struct sembuf unlock3={2,1,0};

typedef struct msgbuf {
	long mtype;
	char line[MAX_SIZE];
}msgbuf;
