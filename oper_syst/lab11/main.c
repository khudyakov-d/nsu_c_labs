#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
	
extern char **environ; 

int putenv(char *string);	

int execvpe( const char *file, char *const *argv, char *const *envp ) {

    for(int i = 0; envp[i] != 0; i++)
	putenv(envp[i]);

    execvp(file, argv);
    
    return 1;
}

int main( int argc, char **argv) {
    char *envp[] = {"COUNTRY=RUSSIA","CAPITAL=MOSCOW",0};

    if(argc == 1){
       printf("Please enter argumets - program name , parametrs...\n");
       return 1;  
    }

    execvpe (argv[1], &argv[1], envp);
    fprintf (stderr, "Invalid command \n" );
    	    
    return 0;
}
