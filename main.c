#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

void see_shell_loop(void) {
    char  *line;
    char **args;
    int    status;
    int    exit;

    do {
        printf("> ");

    } while (exit == 0);
}

int main(int argc, char **argv) {
    see_shell_loop();

    return EXIT_SUCCESS;
}
