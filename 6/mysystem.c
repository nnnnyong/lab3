#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

int mysystem(const char *cmd) {
    pid_t pid; 
    int status, i = 0;
    if (cmd == NULL)
        return 1;

    pid = fork();

    if (pid == 0) {
        execl("/bin/sh", "sh", "-c", cmd, (char *)NULL);
        _exit(127);
    }
    else if (pid > 0 ) {
        waitpid(pid, &status, 0);
    }
    else {
        status = -1;
    }
    return status;
}

int main(int argc, char *argv[]) {
    int i = 0;
    size_t len = 0;

    if (argc < 3){
        fprintf(stderr, "Usage: mysystem command option\n");
    }

    while (i < argc - 1) {
        len = strlen(argv[i]);
        i++;
    }    

    char *cmd = (char *)malloc(sizeof(char) * (len + argc - 2));
    strcpy(cmd, argv[1]);
    for (i = 2; i < argc; i++) {
        strcat(cmd, " ");
        strcat(cmd, argv[i]);
    }
    
    mysystem(cmd);
}