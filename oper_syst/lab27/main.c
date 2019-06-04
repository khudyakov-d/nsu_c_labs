#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <string.h>

int main(int argc, char *argv[]) {
    FILE *pipeFd, *fd;
    char buf[255];
    char *err;

    if (argc != 2) {
        printf("Please enter 1 argument - file name");
        return 1;
    }


    if ((fd = fopen(argv[1], "r")) == 0) {
        perror("fopen");
        exit(1);
    }

    if ((pipeFd = popen("wc -l", "w")) == 0) {
        perror("popen");
        exit(2);
    }


    while (1) {
        err = fgets(buf, 255, fd);

        if (err == NULL) {
            if (feof(fd) != 0) {
                break;
            } else {
                perror("fgets");
                exit(3);
            }
        } else {
            if (strlen(buf) == 1 && strncmp(buf, "\n", 1) == 0) {
                fputs(buf, pipeFd);
            }
        }
    }

    fclose(fd);

    if (pclose(pipeFd) == -1) {
        perror("pclose");
        exit(4);
    }

    return 0;
}