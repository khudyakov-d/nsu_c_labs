#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fnmatch.h>
#include <string.h>
#include <malloc.h>

void findFiles(DIR *dir, char *template, char *path, int *flag) {
    struct dirent *fileRecord = NULL;
    char *fullFileName = NULL;
    char slash[1] = "/";

    do {
        fileRecord = readdir(dir);

        if (NULL != fileRecord) {
            struct stat status;

            fullFileName = (char *) malloc(sizeof(char) * (strlen(path) + strlen(fileRecord->d_name) + 1));
            strcpy(fullFileName, path);
            strcat(fullFileName, fileRecord->d_name);
            fullFileName[strlen(path) + strlen(fileRecord->d_name) + 1] = 0;

            if (-1 != stat(fullFileName, &status) &&
                strcmp(fileRecord->d_name, "..") &&
                strcmp(fileRecord->d_name, ".")) {


                if (S_ISREG(status.st_mode)) {
                    if (0 == fnmatch(template, fullFileName, FNM_PATHNAME)) {
                        printf("%s\n", fileRecord->d_name);
                        *flag = 1;
                    }
                }
                if (S_ISDIR(status.st_mode)) {
                    DIR * localDir;

                    localDir = opendir(fullFileName);


                    fullFileName = (char *) realloc(fullFileName, strlen(fullFileName) + 1);
                    strcat(fullFileName, slash);

                    findFiles(localDir, template, fullFileName, flag);
                }
            }
        }
    } while (fileRecord != NULL);
}

int main() {
    char buf[256];
    DIR *dir = opendir(".");
    int flag = 0;
    char *path = "";

    if (NULL != fgets(buf, 256, stdin)) {
        char *template = (char *) malloc((strlen(buf) - 1) * sizeof(char));
        memcpy(template, buf, strlen(buf) - 1);

        findFiles(dir, template, path, &flag);

        if (0 == flag) {
            template = (char *) realloc(template, strlen(buf));
            template[strlen(buf)] = 0;
            printf("%s", template);
        }
    }


    return 0;
}