#include <stdio.h>
#include <zconf.h>

int main(int argc, char *argv[]) {
    FILE *file;
    printf("Effective id: %d; \nReal id: %d; \n", geteuid(), getuid());

    if (argc == 2) {
        file = fopen(argv[1], "r");
        if (file == NULL) {
            perror("Error. Program can't open file!");
        }
        else fclose(file);
        setuid(geteuid());

        printf("Effective id: %d; \nReal id: %d; \n", geteuid(), getuid());
        file = fopen(argv[1], "r");
        if (file == NULL) {
            perror("Error. Program can't open file!");
        }
    } else {
        printf("Error! Entry one argument - file name!");
        return 1;
    }


    return 0;
}