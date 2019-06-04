#include    <signal.h>
#include    <stdio.h>
#include    <unistd.h>
#include <stdlib.h>
#include <termios.h>


struct termios oldSettings;
struct termios newSettings;

void sigcatch(int sig) {
    static int n;
    signal(sig, SIG_IGN);

    if (sig == SIGINT) {
        n++;
    } else if (sig == SIGQUIT) {
        tcsetattr(fileno(stdin), TCSANOW, &oldSettings);
        printf("Signals count: %d\n", n);
        exit(0);
    }

    signal(sig, sigcatch);
}

int main(void) {

    tcgetattr(fileno(stdin), &oldSettings);
    newSettings = oldSettings;
    newSettings.c_lflag &= ~ECHO;
    newSettings.c_cc[VINTR] = 27;
    tcsetattr(fileno(stdin), TCSANOW, &newSettings);

    signal(SIGINT, sigcatch);
    signal(SIGQUIT, sigcatch);

    while (1) {
    }

    return 0;
}