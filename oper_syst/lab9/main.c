#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <wait.h>

int main(int argc, char **argv) {
    pid_t wpid;
    int status;

    printf("TEST PROGRAM");
    fflush(stdout);

    if (fork() == 0) {
        execl("/bin/cat", "cat", argv[1], (char *) 0);
        exit(1);
    }

    wpid = wait(&status);

    if (wpid == 1) {
        printf("No unfinished subprocesses");
        return -1;
    } else {
        printf("END OF TEST PROGRAM");
    }

    return 0;
}
