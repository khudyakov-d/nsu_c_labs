#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <zconf.h>

#define DETAIL_A 0
#define DETAIL_B 1
#define MODULE 3

int main(int argc, char **argv) {
    int sem = semget(getuid(), 4, 0);
    int num = 0;

    struct sembuf operations[3] = {{DETAIL_A, -1, SEM_UNDO},
                                  {DETAIL_B, -1, SEM_UNDO},
                                  {MODULE,   1,  SEM_UNDO}};


    if(0 == fork())
    {
        execl("lab33part", "a", 0);
        perror("Couldn't start part C executing");
        return 1;
    }

    if(0 == fork())
    {
        execl("lab33part", "b", 0);
        perror("Couldn't start part C executing");
        return 1;
    }

    for (;;) {
        if (-1 == semop(sem, &operations[0], 2)) {
           break;
        }
        printf("Module %d was producted\n", num + 1);
        num++;

        if (-1 == semop(sem, &operations[2], 1)) {
            perror("module");
            break;
        }
    }

    return 0;
}