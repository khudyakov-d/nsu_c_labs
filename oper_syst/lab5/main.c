#include <stdio.h>
#include <malloc.h>
#include <fcntl.h>
#include <zconf.h>

struct StringInfo {
    long offset;
    size_t stringLength;
};

typedef struct StringInfo StringInfo;

long makeValue(const char *string){
    long value = 0;
    for (int i = 0; string[i] != 0; ++i) {
        if (string[i] >= '0' && string[i] <= '9') {
            value *= 10;
            value += string[i] - '0';
        } else {
            return  -1;
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
    char symbol;
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

void printLine(StringInfo *stringInfo, long value, const int outputFile) {
    char string[255] = {};
    lseek(outputFile, stringInfo[value - 1].offset, SEEK_SET);
    read(outputFile, string, stringInfo[value - 1].stringLength);
    string[stringInfo[value - 1].stringLength + 1] = 0;
    printf("%s\n", string);
}

int main(int argc, char *argv[]) {
    int inputFile;
    long stringCount = 0, value = 0,  length = 0;
    const int N = 100;
    char string[255] = {};

    StringInfo *stringInfo = (StringInfo *) malloc(N * sizeof(StringInfo));
    if (stringInfo == NULL){
        printf("Allocation error");
        return 1;
    }

    if (argc != 2){
        printf("Incorrect number of arguments. Please pass one argument - file name.");
        return 1;
    }

    inputFile = open(argv[1], O_RDONLY);
    if (inputFile == -1) {
        perror("Error. Program can't open file!");
        return 1;
    }

    setInfo(stringInfo, &stringCount, inputFile);
    for (int i = 0; i < stringCount; ++i) {
        printf("%ld %ld\n",stringInfo[i].offset,stringInfo[i].stringLength);
    }
    printf("This program prints a line from a file whose name is transferred to the command line\n\
with a number that the user enters.\n\
Your file name: %s\ncount of strings: %ld\n\
Please write a number of the line...\n", argv[1],stringCount);

    fflush(stdout);

    while (1) {
        clearString(string);
        length = read(0, string, sizeof(char) * 255);
        string[length - 1] = 0;

        if(length > 1){
            value = makeValue(string);
        } else {
            value = -1;
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
