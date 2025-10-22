#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAGIC_NUMBER 10
#define SEMKEY 10105
#define SHMKEY 10106

int semid, shmid;

union semun {
  int val;               /* Value for SETVAL */
  struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
  unsigned short *array; /* Array for GETALL, SETALL */
  struct seminfo
      *__buf; /* Buffer for IPC_INFO
                                                            (Linux-specific) */
};
int main(int argc, char **argv) {
  semid = semget(SEMKEY, 2, IPC_CREAT | 0666);
  shmid = shmget(SHMKEY, sizeof(int) * (MAGIC_NUMBER + 1), IPC_CREAT | 0666);
  if (shmid == -1) {
    fprintf(stderr, "unsuccessful shared memory creation\n");
    fprintf(stderr, "%d\n", errno);
    exit(0);
  }
  struct sembuf lock0 = {0, -1, 0};
  struct sembuf unlock0 = {0, 1, 0};
  struct sembuf lock1 = {1, -1, 0};
  struct sembuf unlock1 = {1, 1, 0};
  int *arr = (int *)shmat(shmid, NULL, 0);
  union semun op;
  unsigned short vals[2] = {0, 1};
  op.array = vals;
  semctl(semid, 0, SETALL, op);
  if (arr == NULL) {
    fprintf(stderr, "unsuccessfull shared memory allocation\n");
    exit(0);
  }

  arr[MAGIC_NUMBER]=1;
  int pid = fork();
  if (pid == 0) {
    // child process
    while (arr[MAGIC_NUMBER]) {
      semop(semid, &lock0, 1);
      int sum = 0;
      for (int i = 0; i < MAGIC_NUMBER; i++)
        sum += arr[i];
      printf("%d\n", sum);
      semop(semid, &unlock1, 1);
    }
  } else {
    // parent process
    FILE *file = fopen("brojevi.txt", "r");
    if (!file) {

      fprintf(stderr, "No such file\n");
      exit(0);
    }
    struct sembuf lock = {0, -1, 0};
    struct sembuf unlock = {0, 1, 0};

    while (!feof(file)) {
      semop(semid, &lock1, 1);
      for (int i = 0; i < MAGIC_NUMBER; i++)
        fscanf(file, "%d\n", &arr[i]);
      semop(semid, &unlock0, 1);
    }

    arr[MAGIC_NUMBER]=0;
    wait(NULL);
    shmdt(arr);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
    semctl(semid, 1, IPC_RMID);
    fclose(file);
  }
}
