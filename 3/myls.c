#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>

void ls(char *dirname) {
    DIR *pdir;
    struct dirent *pde;
    char recdir[1024];

    if ((pdir = opendir(dirname)) == NULL) {
        perror("open dir");
        exit(1);
    }

    printf("\n%s:\n", dirname);

    while ((pde = readdir(pdir))) {
        if (pde->d_name[0] != '.')
            printf("%s\t", pde->d_name);
    }
    closedir(pdir);

    if ((pdir = opendir(dirname)) == NULL) {
        perror("open dir");
        exit(1);
    }
    
    while((pde = readdir(pdir))) {
        if (pde->d_name[0] != '.') {
            if (pde->d_type == 4) {
                sprintf(recdir, "%s/%s", dirname, pde->d_name);
                printf("\n");
                ls(recdir);
            }
        }
    }
    closedir(pdir);

    printf("\n");
}
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: myls dir_name\n");
        exit(1);
    }

    ls(argv[1]);

}
