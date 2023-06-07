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

#define SEESH_RL_BUFSIZE 1024
#define SEESH_TOK_BUFSIZE 64
#define SEESH_TOK_DELIM " \t\r\n\a"

void   see_shell_loop(void);
char  *see_sh_read_line_old();
char  *see_sh_read_line();
char **see_sh_split_line(char *line);

char *see_sh_read_line_old() {
    size_t bufsize = SEESH_RL_BUFSIZE;
    int    position = 0;
    char  *buffer = malloc(sizeof(char) * bufsize);
    int    c;

    if (!buffer) {
        fprintf(stderr, "seesh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        c = getchar();

        // EOF or newline -> replace with null character and return
        if (c == EOF || c == '\n') {
            buffer[position] = 0;
            return buffer;
        }

        buffer[position++] = c;

        // if we exceed buffer size reallocate
        if (position >= bufsize) {
            bufsize += SEESH_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);

            if (!buffer) {
                buffer[position] = 0;
                return buffer;
            }
        }
    }
}

char *see_sh_read_line() {
    char  *line = NULL;
    size_t bufsize = 0;

    if (getline(&line, &bufsize, stdin) == -1) {
        if (feof(stdin)) {
            exit(EXIT_SUCCESS);
        }
        perror("Error by readline");
        exit(EXIT_FAILURE);
    }

    return line;
}

void see_shell_loop(void) {
    char  *line;
    char **args;
    int    status;
    int    exit;

    /* int file = open("test.txt", O_RDONLY); */
    /* dup2(file, STDIN_FILENO); */

    do {
        printf("> ");

        line = see_sh_read_line_old();
        printf("%s\n", line);
    } while (exit == 0);
}

char **see_sh_split_line(char *line) {
    int    bufsize = SEESH_TOK_BUFSIZE;
    int    position = 0;
    char **tokens = malloc(bufsize * sizeof(char));
    char  *token;

    if (tokens == NULL) {
        fprintf(stderr, "seesh: allocation errror\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, SEESH_TOK_DELIM);
    while (token != NULL) {
        tokens[position++] = token;

        if (position > bufsize) {
            bufsize += SEESH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char));

            if (tokens == NULL) {
                fprintf(stderr, "seesh: allocation errror\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, SEESH_TOK_DELIM);
    }

    tokens[position] = NULL;
    return tokens;
}

int main(int argc, char **argv) {
    see_shell_loop();

    return EXIT_SUCCESS;
}
