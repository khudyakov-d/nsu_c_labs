#include <stdio.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <zconf.h>

int main(int argc, char** argv)
{
    int sem = semget(getuid(), 4, 0);
    int num = 0;

    unsigned int delay[3] = {2,3,4};

    ushort detailNumber = (ushort)(argv[0][0] - 'a');

    struct sembuf part = {detailNumber, 1, SEM_UNDO};

    for(;;)
    {
        sleep(delay[detailNumber]);
        printf("Detail %c number %d was produced\n", argv[0][0], num + 1);
        num++;

        if (-1 == semop(sem, &part, 1)){
            break;
        }
    }

    return 0;
}
