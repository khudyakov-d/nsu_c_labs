#include <stdio.h>
#include <malloc.h>
#include <fcntl.h>
#include <zconf.h>
#include <signal.h>
#include <errno.h>

extern int errno;

struct StringInfo {
    long offset;
    size_t stringLength;
};

typedef struct StringInfo StringInfo;

void timeout() {
    signal(SIGALRM, timeout);
}

long makeValue(const char *string) {
    long value = 0;
    for (int i = 0; string[i] != 0; ++i) {
        if (string[i] >= '0' && string[i] <= '9') {
            value *= 10;
            value += string[i] - '0';
        } else {
            return -1;
        }
    }
    return value;
}

void clearString(char *string) {
    for (int i = 0; string[i] != 0; ++i) {
        string[i] = 0;
    }
}

void setInfo(StringInfo *stringInfo, long *stringCount, const int inputFile) {
    long fileSize;
    char symbol = 0;
    long index = 0;

    fileSize = lseek(inputFile, 0, SEEK_END);
    lseek(inputFile, 0, SEEK_SET);

    stringInfo[*stringCount].offset = index;

    while (index != fileSize) {
        index += read(inputFile, &symbol, sizeof(char));
        if (symbol == '\n') {
            stringInfo[++(*stringCount)].offset = index;
        } else {
            stringInfo[*stringCount].stringLength++;
        }

        index++;
    }

    if (symbol != '\n'){
        stringInfo[(*stringCount)].offset = index - stringInfo[(*stringCount)++].stringLength;
    }
}

void printFile(StringInfo *stringInfo, long stringCount, const int inputFile) {
    char string[255] = {};

    lseek(inputFile, 0, SEEK_SET);

    for (int i = 0; i < stringCount; ++i) {
        clearString(string);
        read(inputFile, string, stringInfo[i].stringLength+1);
        string[stringInfo[i].stringLength + 1] = 0;
        printf("%d: %s", i+1, string);
    }
    printf("\n");
}

void printLine(StringInfo *stringInfo, long value, const int outputFile) {
    char string[255] = {};
    lseek(outputFile, stringInfo[value - 1].offset, SEEK_SET);
    read(outputFile, string, stringInfo[value - 1].stringLength);
    string[stringInfo[value - 1].stringLength + 1] = 0;
    printf("%s\n", string);
}

int main(int argc, char *argv[]) {
    int inputFile;
    long stringCount = 0, value = 0, length = 0;
    const int N = 100;
    char string[255] = {};

    StringInfo *stringInfo = (StringInfo *) malloc(N * sizeof(StringInfo));
    if (stringInfo == NULL) {
        printf("Allocation error");
        return 1;
    }

    if (argc != 2) {
        printf("Incorrect number of arguments. Please pass one argument - file name.");
        return 1;
    }

    inputFile = open(argv[1], O_RDONLY | O_NDELAY);
    if (inputFile == -1) {
        perror("Error. Program can't open file!");
        return 1;
    }

    setInfo(stringInfo, &stringCount, inputFile);

    printf("This program prints a line from a file whose name is transferred to the command line\
with a number that the user enters.\n\
Your file name: %s\nFile size (count of strings): %ld\n\
Please write a number of the line, you have only 5 seconds\n", argv[1], stringCount);

    fflush(stdout);
    signal(SIGALRM, timeout);

    while (1) {
        clearString(string);
        alarm(5);
        length = read(0, string, sizeof(char) * 255);
        alarm(0);
        if (errno == EINTR) {
            printf("Time is out. The file will be printed...\n");
            printFile(stringInfo, stringCount, inputFile);
            close(inputFile);
            return 1;
        } else {
            string[length - 1] = 0;
            if(length > 1){
                value = makeValue(string);
            } else {
                value = -1;
            }
        }

        if (value == 0) {
            break;
        } else if (value <= stringCount && value > 0) {
            printLine(stringInfo,value,inputFile);
        } else {
            printf("Incorrect input! Please write a number of line no more file size\n");
            fflush(stdout);
        }
    }

    close(inputFile);
    return 0;
}
