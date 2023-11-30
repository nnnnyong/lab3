#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <wait.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

#define SEMKEY (key_t)0111
#define SHMKEY (key_t)0111
#define SHMSIZE 16
#define BUFFSIZE 16

char frbuffer[BUFFSIZE];
char fwbuffer[BUFFSIZE];

void p(int semid);
void v(int semid);
void read_shm(void *shmaddr);
void write_shm(void *shmaddr);
void shmwrite_sem(int semid1, int semid2, void *shmaddr);
void shmread_sem(int semid1, int semid2, void *shmaddr);

union semun {
        int value;
        struct semid_ds *buf;
        unsigned short int *array;
};

int main() {
    int rfd, wfd;
    int shmid, semid1, semid2, len;
    void *shmaddr;
    pid_t pid;
    size_t nread;

    union semun arg1;
    union semun arg2;

    // 파일
    if ((rfd = open("read.txt", O_RDONLY)) == -1) {
        perror("open");
        exit(1);
    }
    if ((wfd = open("write.txt", O_CREAT | O_WRONLY | O_TRUNC, 0666)) == -1) {
        perror("open");
        exit(1);
    }

    // 세마포어
    if ((semid1 = semget(0111, 1, IPC_CREAT | 0666)) == -1) {
        perror("semget failed");
        exit(1);
    }
    if ((semid2 = semget(0112, 1, IPC_CREAT | 0666)) == -1) {
        perror("semget failed");
        exit(1);
    }
    arg1.value = 1;
    arg2.value = 0;
    if (semctl(semid1, 0, SETVAL, arg1) == -1) {
        perror("semctl failed");
        exit(1);
    }
    if (semctl(semid2, 0, SETVAL, arg2) == -1) {
        perror("semctl failed");
        exit(1);
    }

    // 공유메모리
    if ((shmid = shmget(SHMKEY, SHMSIZE, IPC_CREAT | 0666)) == -1) {
        perror("shmget failed");
        exit(1);
    }
    if ((shmaddr = shmat(shmid, NULL, 0)) == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }

    pid = fork();
    if (pid == 0) {
        while ((nread = read(rfd, frbuffer, BUFFSIZE)) > 0) {
            shmwrite_sem(semid1, semid2, shmaddr);
        }
        
        exit(0);
    }
    else if (pid) {
        while(waitpid(pid, (int *)0, WNOHANG) == 0) {
            shmread_sem(semid1, semid2, shmaddr);
            write(wfd, fwbuffer, BUFFSIZE);
        }
    }
    else {
        perror("fork failed");
        exit(1);
    }

    if (semctl(semid1, 0, IPC_RMID, arg1) == -1) {
        perror("semctl failed");
        exit(1);
    }
    if (semctl(semid2, 0, IPC_RMID, arg2) == -1) {
        perror("semctl failed");
        exit(1);
    }
    if (shmdt(shmaddr) == -1) {
        perror("shmdt failed");
        exit(1);
    }
    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        perror("shmctl failed");
        exit(1);
    }
}

void shmwrite_sem(int semid1, int semid2, void *shmaddr) {
    p(semid1);
    v(semid2);
    strcpy((char *)shmaddr, frbuffer);
    v(semid2);
}
void shmread_sem(int semid1, int semid2, void *shmaddr) {
    int i;

    p(semid2);
    if (strcmp((char *)shmaddr, "") != 0) {
        for (i = 0; i < strlen((char *)shmaddr); i++) {
            fwbuffer[i] = ((char *)shmaddr)[i];
        }
        strcpy((char *)shmaddr, "");
    }
    v(semid2);
    v(semid1);
}

void p(int semid) {
    struct sembuf pbuf;
    pbuf.sem_num = 0;
    pbuf.sem_op = -1;
    pbuf.sem_flg = 0;

    if (semop(semid, &pbuf, 1) == -1) {
        perror("semop failed");
        exit(1);
    }
}

void v(int semid) {
    struct sembuf vbuf;
    vbuf.sem_num = 0;
    vbuf.sem_op = 1;
    vbuf.sem_flg = 0;

    if (semop(semid, &vbuf, 1) == -1) {
        perror("semop failed");
        exit(1);
    }
}



