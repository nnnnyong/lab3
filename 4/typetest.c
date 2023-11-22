#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/times.h>
#include <stdlib.h>

int main() {
    int fd, i = 0, j = 0;
    int nread, cnt = 0, errcnt = 0;
    char ch;
    char *text[] = {
        "This is a test",
        "system programming",
        "I have a pen, I have a pencil!"
    };
    
    double types_per_sec, types_per_min, total_sec;
    clock_t tcstart, tcend, cticks;
    struct tms tmstart, tmend;

    struct termios init_attr, new_attr;

    fd = open(ttyname(fileno(stdin)), O_RDWR);
    tcgetattr(fd, &init_attr);

    new_attr = init_attr;
    new_attr.c_lflag &= ~ICANON;
    new_attr.c_lflag &= ~ECHO;
    new_attr.c_cc[VMIN] = 1;
    new_attr.c_cc[VTIME] = 0;
    
    if (tcsetattr(fd, TCSANOW, &new_attr) != 0) {
        fprintf(stderr, "can't set tty attr\n");
    }

    printf("type sentances\n");
    if ((tcstart = times(&tmstart)) == -1) {
        perror("failed to get start time\n");
        exit(1);
    }
    while (i < 3) {
        printf("%s\n", text[i]);
        while((nread = read(fd, &ch, 1)) > 0 && ch != '\n') {
            if (ch == text[i][j++]) {
                write(fd, &ch, 1);
                cnt++;
            }
            else {
                write(fd, "*", 1);
                errcnt++;
            }
        }
        j = 0;
        i++;
        printf("\n");
    }
    if ((tcend = times(&tmend)) == -1) {
        perror("failed to get end time\n");
        exit(1);
    }

    tcend = tcend - tcstart; 
    total_sec = (double)tcend / sysconf(_SC_CLK_TCK);
    types_per_min =  cnt / total_sec * 60;
    
    printf("\nerror count : %d\n", errcnt);
    printf("types per min : %.2f\n", types_per_min);

    tcsetattr(fd, TCSANOW, &init_attr);
    close(fd);
}