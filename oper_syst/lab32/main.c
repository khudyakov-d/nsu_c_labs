#include <stdio.h>
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
    int queueId, pid, childCount, status = 0;

    queueId = msgget(getuid(), IPC_CREAT | 0666);

    if (queueId == -1) {
        perror("Couldn't create queue");
        return 1;
    }

    printf("Please enter count of senders: ");
    scanf("%d", &childCount);

    for (int i = 0; i < childCount; i++) {
        pid = fork();
        if (-1 == pid) {
            perror("Couldn't create process");
            return 1;

        } else if (0 == pid) {
            execl("lab32_1", 0);
            perror("Couldn't exec process");
            return 1;

        }
    }

    while (status < childCount) {

        struct msgbuffer msg = {};

        if (-1 != msgrcv(queueId, &msg, M_SIZE, 0, 0)) {
            if (!strcmp(msg.mtext, "STOP")) {
                status++;
            } else {
                printf("message: \" %s \" was received from process number: %ld", msg.mtext, msg.mtype);
            }
        }
    }


    msgctl(queueId, IPC_RMID, 0);

    return 0;
}
