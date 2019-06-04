#include <stdio.h>
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
    long err;

    queueId = msgget(getuid(), 0);

    if (queueId == -1) {
        perror("Couldn't find queue");
        return -1;
    }

    while (1) {
        struct msgbuffer msg = {};

        err = msgrcv(queueId, &msg, M_SIZE, 1, 0);

        if (-1 == err) {
            break;
        }

        printf("msg: \"%s\" type: %ld\n", msg.mtext, msg.mtype);
        if (!strcmp(msg.mtext, "EOF")) {
            char msgBuf[5] = "STOP";
            memcpy(msg.mtext,msgBuf, sizeof(msgBuf));

            if (-1 == msgsnd(queueId, &msg, sizeof(long) + strlen(msgBuf), 0)) {
                perror("Couldn't send message");
                return 1;
            }
            break;
        }

    }

    return 0;
}