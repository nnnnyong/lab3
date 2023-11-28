#include <stdio.h>
#include <stdlib.h>

#define BUFFSIZE 32

char rbuff[BUFFSIZE];
char wbuff[BUFFSIZE];

int main() {
    int p[2];
    int pid;

    if (pipe(p) == -1) {
        perror("pipe call failed");
        exit(1);
    }

    pid = fork();
    if (pid == 0) {
        close(p[0]);

        printf("write string : ");
        scanf("%s", wbuff);

        write(p[1], wbuff, BUFFSIZE);
    }
    else if (pid > 0) {
        close(p[1]);

        read(p[0], rbuff, BUFFSIZE);
        printf("read string : %s\n", rbuff);
    }
    else {
        perror("fork failed");
        exit(1);
    }
}