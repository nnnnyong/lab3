#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <termios.h>

int main() {
    int fd, i = 0;
    int nread, cnt = 0, errcnt = 0;
    char ch;
    char *text[] = {
        "This is a test",
        "system programming",
        "I have a pen, I have a pencil!"
    };
    
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
    while (i < 3) {
        printf("%s\n", text[i]);
        while((nread = read(fd, &ch, 1)) > 0 && ch != '\n') {
            if (ch == text[i][cnt++]) {
                write(fd, &ch, 1);
            }
            else {
                write(fd, "*", 1);
                errcnt++;
            }
        }
        cnt = 0;
        i++;
        printf("\n");
    }

    printf("\nerror count : %d\n", errcnt);
    tcsetattr(fd, TCSANOW, &init_attr);
    close(fd);
}