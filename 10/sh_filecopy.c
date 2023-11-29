#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <wait.h>
#include <string.h>
#include <signal.h>

#define SEMKEY (key_t)0111
#define SHMKEY (key_t)0111
#define SHMSIZE 16
#define BUFFSIZE 16

char frbuffer[BUFFSIZE];
char fwbuffer[BUFFSIZE];

void shmread_sem(int semid, void *shmaddr);
void shmwrite_sem(int semid, void *shmaddr);
void p(int semid);
void v(int semid);

int main() {
    int rfd, wfd;
    int shmid, semid, len;
    void *shmaddr;
    pid_t pid;
    size_t nread;
    int s;

    union semun {
        int value;
        struct semid_ds *buf;
        unsigned short int *array;
    } arg;

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
    if ((semid = semget(SEMKEY, 1, IPC_CREAT | 0666)) == -1) {
        perror("semget failed");
        exit(1);
    }
    arg.value = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
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
            shmwrite_sem(semid, shmaddr);
        }
        exit(0);
    }
    else if (pid) {
            if (waitpid(pid, (int *)0, WNOHANG) == 0)
                shmread_sem(semid, shmaddr);
    }
    else {
        perror("fork failed");
        exit(1);
    }


    if (semctl(semid, 0, IPC_RMID, arg) == -1) {
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

void shmwrite_sem(int semid, void *shmaddr) {
    p(semid);
        strcpy((char *)shmaddr, frbuffer);
    v(semid);
}

void shmread_sem(int semid, void *shmaddr) {
    int len, i;
    while(1) {
        p(semid);
            
        if ((len = strlen((char *)shmaddr)) == 0)
            return;
    
        for (i = 0; i < strlen((char *)shmaddr); i++) {
            fwbuffer[i] = ((char *)shmaddr)[i];
        }

        strcpy((char *)shmaddr, "\0");
        v(semid);
    }
}

void p(int semid) {
    struct sembuf pbuf;
    pbuf.sem_num = 0;
    pbuf.sem_op = -1;
    pbuf.sem_flg = IPC_NOWAIT;

    if (semop(semid, &pbuf, 1) == -1) {
        perror("semop failed");
        exit(1);
    }
}

void v(int semid) {
    struct sembuf vbuf;
    vbuf.sem_num = 0;
    vbuf.sem_op = 1;
    vbuf.sem_flg = IPC_NOWAIT;

    if (semop(semid, &vbuf, 1) == -1) {
        perror("semop failed");
        exit(1);
    }
}

void read_shm(void *shmaddr) {
    int i;
    int len;

    if ((len = strlen((char *)shmaddr)) == 0)
        return;
    for (i = 0; i < strlen((char *)shmaddr); i++) {
        fwbuffer[i] = ((char *)shmaddr)[i];
    }
}

void write_shm(void *shmaddr) {
    strcpy((char *)shmaddr, frbuffer);
}

