#include <stdio.h>
#include <zconf.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define M_SIZE 256

struct msgbuffer {
    long mtype;
    char mtext[M_SIZE];
};


int main() {
    int queueId;
    queueId = msgget(getuid(), IPC_CREAT | 0666);
    if (queueId == -1) {
        perror("Couldn't create queue");
        return 1;
    }

    while (1) {
        char msgBuf[M_SIZE];
        struct msgbuffer msg = {};

        scanf("%s", msgBuf);
        memcpy(msg.mtext, msgBuf, strlen(msgBuf));
        msg.mtype = 1L;

        if (-1 == msgsnd(queueId, &msg, sizeof(long) + strlen(msgBuf), 0)) {
            perror("Couldn't send message");
            break;
        }

        if (strcmp(msgBuf, "0") == 0)
            break;
    }

    msgctl(queueId, IPC_RMID, 0);

    return 0;
}