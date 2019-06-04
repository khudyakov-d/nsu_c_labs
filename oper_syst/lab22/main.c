#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <zconf.h>

#define MAX_COUNT 100
#define STRING_SIZE 256

int readStatus = 0;

void timeout(int sig) {
    signal(sig, timeout);
    readStatus = 1;
}

int main(int argc, char **argv) {
    FILE *files[MAX_COUNT];

    int endStatus = 0, fdNumber = 0;
    const int filesCount = argc - 1;

    char string[STRING_SIZE] = {};
    char *res = NULL;

    if (1 == argc) {
        printf("Incorrect number of arguments. Please pass arguments - files names.");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        files[i - 1] = fopen(argv[i], "r");
        if (files[i - 1] == NULL) {
            perror("Couldn't open a file");
            return -1;
        }
    }

    signal(SIGALRM, timeout);

    while (endStatus != filesCount) {

        alarm(5);
        if (NULL != files[fdNumber]) {
            res = fgets(string, STRING_SIZE, files[fdNumber]);
        }
        alarm(0);

        if (readStatus == 1) {
            fdNumber = (fdNumber + 1) % filesCount;
        } else {
            if (NULL == res) {
                endStatus++;
                fclose(files[fdNumber]);
                files[fdNumber] = NULL;
                fdNumber = (fdNumber + 1) % filesCount;
            } else {
                printf("%s ", string);
            }
        }

        readStatus = 0;
    }


    return 0;
}