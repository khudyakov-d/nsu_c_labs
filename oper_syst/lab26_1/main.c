#include <stdio.h>
#include <zconf.h>
#include <ctype.h>
#include <string.h>

int main() {
    long  readStatus;
    char buf[255];

    while (1) {
        readStatus = read(0, buf, 255);

        if (readStatus <= 0 || ((readStatus == 2) && (strncmp(buf, "0\n",2) == 0))) {

            break;
        }

        for (int i = 0; i < readStatus; i++)
            buf[i] = (char) toupper(buf[i]);

        write(1, buf, readStatus);
    }

    return 0;
}