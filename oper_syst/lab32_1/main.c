#include <stdio.h>
#include <stdio.h>
#include <zconf.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>

#define M_SIZE 256

struct msgbuffer {
    long mtype;
    char mtext[M_SIZE];
};

int main() {
    int queueId, loadingPoint;
    long err;

    queueId = msgget(getuid(), 0);

    if (queueId == -1) {
        perror("Couldn't find queue");
        return -1;
    }


    loadingPoint = rand() % 5 + 5;
    for (int i = 0; i < loadingPoint; ++i) {

        struct msgbuffer msg = {};
        sprintf(msg.mtext, "loading progress %d/%d", i, loadingPoint);
        msg.mtype = getpid();

        if (-1 == msgsnd(queueId, &msg, sizeof(long) + strlen(msg.mtext), 0)) {
            perror("Couldn't send message");
            return 1;
        }
    }

    char msgbuf[5] = "STOP";
    struct msgbuffer msg = {};
    memcpy(msg.mtext, msgbuf, 5);
    msg.mtype = getpid();

    if (-1 == msgsnd(queueId, &msg, sizeof(long) + strlen(msg.mtext), 0)) {
        perror("Couldn't send message");
        return 1;
    }


    return 0;
}