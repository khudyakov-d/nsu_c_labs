#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define COUNT 10
#define MAX 100
#define NUM_SIZE 4

int main() {
    char buf[NUM_SIZE];

    FILE *ptr[2];

    srand(time(NULL));

    if (-1 == p2open("/bin/sort -nb", ptr)) {
        perror("Can't create pipe");
        return -1;
    }

    for (int i = 0; i < COUNT; i++) {
        fprintf(ptr[0], "%d\n", rand() % MAX);
    }

    if (-1 == fclose(ptr[0])) {
        perror("Can't close input file");
        return -2;
    }

    
    while (fgets(buf, NUM_SIZE, ptr[1])) {
        printf("%s\n", buf);
    }


    if (-1 == fclose(ptr[1])) {
        perror("Can't close output file");
        return -3;
    }

    putchar('\n');
    return 0;
}