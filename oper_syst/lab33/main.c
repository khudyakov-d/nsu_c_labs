#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <signal.h>
#include <zconf.h>
#include <stdlib.h>

#define WIDGETS_COUNT 10
#define C_DETAIL 2
#define MODULE 3


int main(int argc, char** argv)
{
	int sem;
	int arg[4] = {1,1,1,1};
	struct sembuf operations[2] = {{C_DETAIL, -1, SEM_UNDO},{MODULE, -1, SEM_UNDO}};

	semctl (sem, 0, SETALL, arg);

	if(-1 == (sem = semget(getuid(), 4, IPC_CREAT | 0666)))
	{
		perror("Couldn't create semaphores");
		return 1;
	}

	if(0 == fork())
	{
		execl("lab33part", "c", 0);
		perror("Couldn't start part C executing");
		return 1;
	}

	if(0 == fork())
	{
		execl("lab33module", 0);
		perror("Couldn't start part C executing");
		return 1;
	}

	for(int i = 0; i < WIDGETS_COUNT; i++)
	{
		if(semop(sem, &operations[0], 2))
		{
			perror("Couldn't make operation with semaphores");
			return 1;
		}
		printf("Widget number %d is done\n", i+1);
	}

	semctl(sem, 0, IPC_RMID, 0);
}
