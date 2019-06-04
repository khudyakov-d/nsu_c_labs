#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <wait.h>
#include <errno.h>

extern int errno;

int main(int argc, char **argv) {
    pid_t wpid;
    int status;

    
    if (argc == 1) {
       printf("Please enter argumets - program name , parametrs...\n");
       return 1;
    }
    
    if (fork() == 0) {
        execvp(argv[1], &argv[1]);
        fprintf(stderr, "Bad program name \n");
	exit(127);
    }

    wpid = wait(&status);

    if (wpid == -1) {
	if(errno == ECHILD){
            printf("No unfinished subprocesses \n");
	}
	else {
	    printf("Subprocess was interrupted by a signal with number: %d",  WTERMSIG(status));
	}
    } else {
        printf("Process exit code: %d \n", WEXITSTATUS(status));
    }
    
    return 0;
}
