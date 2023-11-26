#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <stdlib.h>

void exitfunc() {
    printf("exit message\n");
}

int main() {
    int status, exit_st;
    pid_t pid;

    atexit(exitfunc);

    char *arg[] = {"mkdir", "test", '\0'};

    if ((pid = fork()) == 0) {
        execvp(arg[0], arg);
    }
    else if (pid > 0) {
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            exit_st = WEXITSTATUS(status);
            printf("exit status : %d\n", exit_st);
        }
    }
    else {
        perror("fork failed");
        exit(1);
    }
}