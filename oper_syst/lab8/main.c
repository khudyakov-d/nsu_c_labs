#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <memory.h>
#include <zconf.h>


int main(int argc, char **argv) {
    struct flock lock;
    int fd;
    char command[20] =  "vi ";

    fd = open(argv[1], O_RDONLY);

    if (2 != argc) {
        printf("Invalid arguments. Please type one argument - file name");
        return 1;
    }

    if (-1 == fd) {
        perror("Can't open file");
        return 1;
    }

    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    if (-1 == fcntl(fd, F_SETLK, &lock)) {
        printf("File capture failed");
        return 1;
    } else {
         strcat (command, argv[1]) ;
         system(command);
    }

    close(fd);
    return 0;
}