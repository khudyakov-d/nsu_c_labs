#include <stdio.h>
#include <mpi.h>
#include <zconf.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>

int send_status = 0;

#define LISTS_COUNT 10
#define TASKS_COUNT 20

enum Operation {
    COUNT_REQUEST, TASK_REQUST, STOP_THREAD, TASK_REFUSING
};

struct ThreadInfo {

    int *localTasks;
    double localRes;
    int tasksCount;
    int curTask;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

typedef struct ThreadInfo ThreadInfo;

void makeTasks(ThreadInfo *info) {
    int j = 0;
    while (info->curTask != info->tasksCount) {

        while (j < info->localTasks[info->curTask]) {
            if (send_status == 0) {
                info->localRes += sqrt(j);
                j++;
            }
        }
        info->curTask++;
    }
}

int takeNewTasks(int size, int rank, struct ThreadInfo *info) {
    enum Operation operation;
    int remainingTasksCount;
    MPI_Request request[2];
    MPI_Status status;
    for (int j = 0; j < size; j++) {

        if (j != rank) {
            printf("rank %d send to %d count requset\n", rank, j);
            fflush(stdout);

            operation = COUNT_REQUEST;
            remainingTasksCount = 0;

            MPI_Isend(&operation, 1, MPI_INT, j, 1, MPI_COMM_WORLD, &request[0]);

            MPI_Irecv(&remainingTasksCount, 1, MPI_INT, j, 2, MPI_COMM_WORLD, &request[1]);
            MPI_Wait(&request[1], &status);

            if (remainingTasksCount > 1) {

                operation = TASK_REQUST;

                printf("rank %d send to %d task requset\n", rank, j);
                fflush(stdout);

                MPI_Isend(&operation, 1, MPI_INT, j, 1, MPI_COMM_WORLD, &request[0]);
                MPI_Irecv(info->localTasks, remainingTasksCount / 2, MPI_INT, j, 2, MPI_COMM_WORLD, &request[1]);
                MPI_Wait(&request[1], &status);

                printf("rank %d take tasks answer from %d\n", rank, j);
                fflush(stdout);

                pthread_mutex_lock(&info->mutex);
                info->curTask = 0;
                info->tasksCount = remainingTasksCount / 2;
                pthread_mutex_unlock(&info->mutex);

                return 1;
            } else {
                printf("rank %d send to %d task refusing\n", rank, j);
                operation = TASK_REFUSING;
                MPI_Isend(&operation, 1, MPI_INT, j, 1, MPI_COMM_WORLD, &request[0]);
            }
        }
    }
    return 0;
}

void setLocalTasksCount(int threadsCount, int tasksCount, int *localTasksCount) {
    int tasksPerThread = tasksCount / threadsCount;
    int remainingTasks = tasksCount % threadsCount;

    for (int i = 0; i < threadsCount; ++i) {
        localTasksCount[i] = tasksPerThread;
    }

    localTasksCount[threadsCount - 1] += remainingTasks;
}


void fillLocalTasks(int rank, int threadsCount, int *localTasks, int tasksCount, int iterationNumber) {
    srand(time(NULL));

    for (int i = 0; i < tasksCount; ++i) {
        localTasks[i] = (900 + rand() % 1000) * abs(rank - (iterationNumber % threadsCount)) + rand() % 1000;
    }

}

void *threadExecutor(void *info) {
    int res = 0, size, rank;
    enum Operation operation;

    makeTasks(((ThreadInfo *) info));
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    printf("end of making tasks %d \n", rank);
    fflush(stdout);

    while (1) {
        res = takeNewTasks(size, rank, (ThreadInfo *) info);
        printf("res %d rank %d \n", res, rank);
        fflush(stdout);

        if (res == 0) {

            printf("exit rank %d \n", rank);
            fflush(stdout);
            break;
        } else {
            makeTasks(((ThreadInfo *) info));
        }

    }


    MPI_Barrier(MPI_COMM_WORLD);
    operation = STOP_THREAD;
    MPI_Ssend(&operation, 1, MPI_INT, rank, 1, MPI_COMM_WORLD);

}

void *threadHandler(void *info) {

    int size, rank, flag = 0, remainingTasksCount;
    int *offset;

    MPI_Status status;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    while (!flag) {
        enum Operation operation;
        MPI_Request request;

        MPI_Recv(&operation, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
        pthread_mutex_lock(&((ThreadInfo *) info)->mutex);

        printf("oper %d  rank %d \n", operation, rank);
        fflush(stdout);

        remainingTasksCount = ((ThreadInfo *) info)->tasksCount - ((ThreadInfo *) info)->curTask;

        switch (operation) {
            case COUNT_REQUEST:

                //printf("count  rank %d \n",  rank);
                fflush(stdout);

                MPI_Send(&remainingTasksCount, 1, MPI_INT, status.MPI_SOURCE, 2, MPI_COMM_WORLD);

                send_status = 1;
                pthread_mutex_unlock(&((ThreadInfo *) info)->mutex);

                break;
            case TASK_REQUST:
                offset = ((ThreadInfo *) info)->localTasks + ((ThreadInfo *) info)->curTask;

                ((ThreadInfo *) info)->curTask += remainingTasksCount / 2;
                MPI_Send(offset, remainingTasksCount / 2, MPI_INT, status.MPI_SOURCE, 2,
                         MPI_COMM_WORLD);

                printf("task answer %d \n", rank);
                fflush(stdout);

                send_status = 0;
                pthread_mutex_unlock(&((ThreadInfo *) info)->mutex);

                break;
            case TASK_REFUSING:
                send_status = 0;
                pthread_mutex_unlock(&((ThreadInfo *) info)->mutex);

                break;
            case STOP_THREAD:
                flag = 1;
                pthread_mutex_unlock(&((ThreadInfo *) info)->mutex);
                break;
        }
    }

}


int main(int argc, char **argv) {
    int providedLevel;
    int size, rank;
    int iterationNumber = 0;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &providedLevel);

    if (MPI_THREAD_MULTIPLE != providedLevel) {
        printf("Error: MPI_THREAD_MULTIPLE level is not supported\n");
        return -1;
    }

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    pthread_attr_t attrs;
    pthread_t threads[2] = {0};
    pthread_mutex_t mutex = {0};
    pthread_cond_t cond = {0};

    int *localTasksCount = (int *) malloc(sizeof(int) * size);
    ThreadInfo threadInfo = {0};

    if (0 == rank) {
        setLocalTasksCount(size, TASKS_COUNT, localTasksCount);
    }

    MPI_Scatter(localTasksCount, 1, MPI_INT, &threadInfo.tasksCount, 1, MPI_INT, 0, MPI_COMM_WORLD);
    threadInfo.localTasks = (int *) malloc(threadInfo.tasksCount * sizeof(int));

    if (0 != pthread_attr_init(&attrs)) {
        perror("Cannot initialize attributes");
        return -1;
    } else {
        if (0 != pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE)) {
            perror("Error in setting attributes");
            return -1;
        }
    }

    while (iterationNumber != LISTS_COUNT) {
        if (rank == 0){
            printf("START OF LIST NUMBER %d \n", iterationNumber);
            printf("VALUES: \n");
            fflush(stdout);
        }

        MPI_Scatter(localTasksCount, 1, MPI_INT, &threadInfo.tasksCount, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);

        fillLocalTasks(rank, size, threadInfo.localTasks, threadInfo.tasksCount, iterationNumber);
        MPI_Barrier(MPI_COMM_WORLD);

        for (int j = 0; j < threadInfo.tasksCount; ++j) {
            printf("%d ", threadInfo.localTasks[j]);
        }
        printf("\n");
        MPI_Barrier(MPI_COMM_WORLD);

        threadInfo.mutex = mutex;
        threadInfo.cond = cond;
        threadInfo.curTask = 0;

        if (0 != pthread_create(&threads[0], &attrs, threadHandler, &threadInfo)) {
            perror("Cannot create a thread");
            return -2;
        }

        if (0 != pthread_create(&threads[1], &attrs, threadExecutor, &threadInfo)) {
            perror("Cannot create a thread");
            return -2;
        }

        for (int i = 0; i < 1; i++) {
            if (0 != pthread_join(threads[i], NULL)) {
                perror("Cannot join a thread");
                return 1;
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);
        if (rank == 0){
            printf("END OF LIST NUMBER %d \n", iterationNumber);
            fflush(stdout);
        }
        ++iterationNumber;
    }


    MPI_Finalize();
    return 0;
}