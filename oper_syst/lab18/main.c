#include <stdio.h>
#include <sys/stat.h>
#include <pwd.h>
#include <malloc.h>
#include <grp.h>
#include <time.h>
#include <memory.h>
#include <libgen.h>

int main(int argc, char **argv) {

    if (argc == 1) {
        printf("Please enter arguments - names of files");
        return -1;
    }

    for (int i = 1; i < argc; ++i) {
        struct stat status;
        long fileSize = -1;


        if (-1 == stat(argv[i], &status)) {
            if (S_ISREG(status.st_mode)) {
                printf("d");
                fileSize = status.st_size;
            } else if (S_ISDIR(status.st_mode)) {
                printf("-");
            } else {
                printf("?");
            }

            printf("%c", (status.st_mode & S_IRUSR) ? 'r' : '-');
            printf("%c", (status.st_mode & S_IWUSR) ? 'w' : '-');
            printf("%c", (status.st_mode & S_IXUSR) ? 'x' : '-');

            printf("%c", (status.st_mode & S_IRGRP) ? 'r' : '-');
            printf("%c", (status.st_mode & S_IWGRP) ? 'w' : '-');
            printf("%c", (status.st_mode & S_IXGRP) ? 'x' : '-');

            printf("%c", (status.st_mode & S_IROTH) ? 'r' : '-');
            printf("%c", (status.st_mode & S_IWOTH) ? 'w' : '-');
            printf("%c", (status.st_mode & S_IXOTH) ? 'x' : '-');


            struct passwd *password = getpwuid(status.st_uid);
            if (NULL != password) {
                printf("%10s ", password->pw_name);
            } else {
                perror("Can't get user name");
            }

            struct group *group = getgrgid(status.st_gid);
            if (NULL != group) {
                printf("%10s ", group->gr_name);
            } else {
                perror("Can't get group name");
            }

            if (fileSize != -1) {
                printf("5%ld", fileSize);
            } else {
                printf("     ");
            }

            char *time = ctime(&status.st_mtime);
            time[strlen(time) - 1] = '\0';
            printf("%s ", time);

            char *name = (char *) malloc(strlen(argv[i]));
            if (NULL != name) {
                strcpy(name, argv[i]);
                printf("%s", basename(name));
                free(name);
            } else {
                perror("Couldn't allocate memory");
            }

            printf("\n");

        } else {
            perror("Can't get stat information");
        }
    }


    return 0;
}
