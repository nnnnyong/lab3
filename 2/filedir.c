#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF 256

int main() {
    int fd1, fd2;
    ssize_t nread = 0;
    char ch;
    char new_dir[MAX_BUF];
    char buf[MAX_BUF];
    char text[200];

    struct stat st;
    mode_t mode = S_IRWXU | S_IRGRP | S_IROTH;

    DIR *pdir;
    struct dirent *pde;

    // 현재 디렉토리 정보
    memset(buf, 0, MAX_BUF);
    if (getcwd(buf, MAX_BUF) == NULL) {
        perror("getcwd");
        exit(1);
    }
    printf("working directory : %s\n", buf);

    if ((pdir = opendir(buf)) == NULL) {
        perror("opendir\n");
        exit(1); 
    }

    // 생성할 디렉토리 입력
    printf("enter new dirname : ");
    scanf("%s", new_dir);

    // 디렉토리가 이미 존재하는지 확인
    while((pde = readdir(pdir)) != NULL) {
        if (pde->d_name == new_dir) {
            printf("존재하는 디렉토리\n");
            exit(1);
        }
    }
    
    closedir(pde);

    if (mkdir(new_dir, 0771) == -1) {
        perror("mkdir");
        exit(1);
    }
    if (chdir(new_dir) == -1) {
        perror("chdir");
        exit(1);
    }

    // 이동한 디렉토리 정보
    memset(buf, 0, MAX_BUF);
    if (getcwd(buf, MAX_BUF) == NULL) {
        perror("getcwd");
        exit(1);
    }
    printf("현재 디렉토리 : %s\n", buf);

    // file1, file2를 생성
    if ((fd1 = creat("file1", mode)) == -1) {
        perror("creat");
        exit(1);
    }    
    umask(044);
    if ((fd2 = open("file2", O_RDWR | O_CREAT, mode)) == -1) {
        perror("open");
        exit(1);
    }

    // 0번 파일에서 읽어서 fd1로 출력
    while((nread = read(0, &ch, 1)) > 0) {
        write(fd1, &ch, 1);
    }
    if (nread < 0) {
        perror("read1");
        exit(1);
    }

    // fd1을 1번으로 복사
    if (dup2(fd2, 1) == -1) {
        perror("dup2");
        exit(1);
    }
    
    printf("this is a test\n");

    // fd2 파일 사이즈 늘리기
    ch = '.';
    lseek(fd2, (off_t)30, SEEK_END);
    write(fd2, &ch, 1);

    // fd2 파일 정보
    fstat(fd2, &st);
    printf("file2 size : %d\n", st.st_size);

    
    close(fd1);
    close(fd2);
}