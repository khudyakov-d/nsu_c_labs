#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <wait.h>

int main(int argc, char *argv[]) {
    int fd[2];
    char buff[256];
    long readStatus;

    if (pipe(fd) < 0) {
        perror("pipe");
        exit(2);
    }

    if (fork() == 0) {
        close(fd[0]);

        while (fgets(buff, 256, stdin)) {
            write(fd[1], buff, strlen(buff));
        }
        close(fd[1]);
        return 0;
    }

    if (fork() == 0) {
        close(fd[1]);
        int flag = 0;

        while (1) {
            readStatus = read(fd[0], buff, 255);

            if (readStatus <= 0) {
                flag = 1;

                break;
            }

            for (int i = 0; i < readStatus; i++)
                buff[i] = (char) toupper(buff[i]);

            write(1, buff, readStatus);
        }

        close(fd[0]);

        return 0;
    }


    while (wait(0) == -1){
        printf("sadasda");
    };

    close(fd[0]);
    close(fd[1]);

    return 0;
}
