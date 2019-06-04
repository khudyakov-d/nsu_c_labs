#include <stdio.h>
#include <malloc.h>
#include <fcntl.h>
#include <zconf.h>
#include <sys/mman.h>
#include <memory.h>
#include <signal.h>
#include <errno.h>

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

void setInfo(StringInfo *stringInfo, long *stringCount, const char *fileData, int fileSize) {
    long index = 0;
    int i;

    stringInfo[*stringCount].offset = index;

    for (i = 0; i < fileSize; ++i) {
        if (fileData[i] == '\n') {
            stringInfo[++(*stringCount)].offset = i + 1;
        } else {
            stringInfo[*stringCount].stringLength++;
        }

    }

    if (fileData[i] != '\n') {
        stringInfo[(*stringCount)].offset = index - stringInfo[(*stringCount)++].stringLength;
    }
}

void printLine(StringInfo *stringInfo, long value, const char * fileData) {
    char string[255] = {};
    memcpy(string,fileData+stringInfo[value-1].offset,stringInfo[value-1].stringLength);
    printf("%s\n", string);
}


int main(int argc, char *argv[]) {
    int inputFile;
    long stringCount = 0, value = 0, length = 0, fileSize;
    const int N = 100;
    char string[255] = {};
    char *fileData;
    StringInfo *stringInfo = (StringInfo *) malloc(N * sizeof(StringInfo));


    if (stringInfo == NULL) {
        printf("Allocation error");
        return 1;
    }

    if (argc != 2) {
        printf("Incorrect number of arguments. Please pass one argument - file name.");
        return 1;
    }

    inputFile = open(argv[1], O_RDONLY);
    if (inputFile == -1) {
        perror("Error. Program can't open file!");
        return 1;
    }

    fileSize = lseek(inputFile, 0, SEEK_END);

    if (fileSize == -1L) {
        printf("Seek error!");
        return 1;
    } else {
        fileData = (char *) mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE, inputFile, 0);
        if(fileData == NULL){
            printf("Error. Program can't read the file");
            return 1;
        }
        setInfo(stringInfo,&stringCount,fileData,fileSize);
    }


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
            printf("%s",fileData);
            fflush(stdout);
            munmap(fileData,fileSize);
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
            printLine(stringInfo,value,fileData);
        } else {
            printf("Incorrect input! Please write a number of line no more file size\n");
            fflush(stdout);
        }
    }

    munmap(fileData,fileSize);
    free(stringInfo);
    return 0;
}