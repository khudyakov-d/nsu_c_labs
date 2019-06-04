#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fnmatch.h>
#include <string.h>
#include <malloc.h>

int main() {
    char buf[256];
    DIR *dir = opendir(".");
    int flag = 0;
    struct dirent *fileRecord = NULL;

    if (NULL != fgets(buf, 256, stdin)) {
        char *template = (char *) malloc((strlen(buf) - 1) * sizeof(char));
        memcpy(template, buf, strlen(buf) - 1);

        if (NULL != dir) {
            do {
                fileRecord = readdir(dir);

                if (NULL != fileRecord) {
                    struct stat status;
                    if (-1 != stat(fileRecord->d_name, &status)) {
                        if (S_ISREG(status.st_mode)) {
                            if (0 == fnmatch(template, fileRecord->d_name, 0)) {
                                printf("%s\n", fileRecord->d_name);
                                flag = 1;
                            }
                        }
                    } else {
                        perror("Can't get file status");
                    }
                }
            } while (fileRecord != NULL);

            if (0 == flag) {
                template = (char *) realloc(template, strlen(buf));
                template[strlen(buf)] = 0;
                printf("%s", template);
            }

        } else {
            perror("Can't get dir status");
        }
    }

    return 0;
}