#include <stdio.h>
#include <zconf.h>
#include <memory.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <malloc.h>

#define M_SIZE 256

struct msgbuffer {
    long mtype;
    char mtext[M_SIZE];
};


int main() {
    int queueId, pid, childCount, status;
    int *pids;
    char endMessage[4] = "EOF";

    queueId = msgget(getuid(), IPC_CREAT | 0666);

    if (queueId == -1) {
        perror("Couldn't create queue");
        return 1;
    }

    printf("Please enter count of receivers: ");
    scanf("%d", &childCount);

    pids = (int *) malloc(sizeof(int) * childCount);
    if (pids == NULL) {
        perror("Couldn't allocate memory");
        return 1;
    }

    for (int i = 0; i < childCount; i++) {
        pid = fork();
        if (-1 == pid) {
            perror("Couldn't create process");
            return 1;

        } else if (0 == pid) {
            execl("lab31recv", 0);
            perror("Couldn't exec process");
            return 1;

        } else pids[i] = pid;
    }

    while (1) {
        char msgBuf[M_SIZE];
        struct msgbuffer msg = {};

        if (EOF == (status = scanf("%s", msgBuf))) {
            memcpy(msg.mtext, endMessage, strlen(msgBuf));
            break;
        } else {
            memcpy(msg.mtext, msgBuf, strlen(msgBuf));
        }

        for (int i = 0; i < childCount; i++) {
            msg.mtype = pids[i];
            if (-1 == msgsnd(queueId, &msg, sizeof(long) + strlen(msgBuf), 0)) {
                perror("Couldn't send message");
                msgctl(queueId, IPC_RMID, 0);
                return 1;
            }
        }

        if (EOF == status) {
            break;
        }
    }


    for (int i = 0; i < childCount; i++) {
        struct msgbuffer msg = {};

        if (-1 != msgrcv(queueId, &msg, M_SIZE, pids[i]), 0) {
            if (strcmp(msg.mtext, "STOP")) {
				break;
            }
        }
    }

    msgctl(queueId, IPC_RMID, 0);

    return 0;
}
