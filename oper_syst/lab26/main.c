#include <stdio.h>
#include <stdlib.h>
#include <zconf.h>
#include <string.h>

int main() {
    FILE *fd;
    char buf[255];

    if (0 == (fd = popen("./lab26_1", "w"))) {
        perror("popen");
        exit(1);
    }

    while (fgets(buf, 256, stdin)) {
        write(fileno(fd), buf, strlen(buf));

        if (strlen(buf) == 2 && (strncmp(buf, "0\n",2) == 0)) {
            break;
        }
    }

    if (pclose(fd) == -1) {
        perror("pclose");
        exit(2);
    }

    return 0;
}