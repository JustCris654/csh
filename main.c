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
#include <sys/wait.h>
#include <unistd.h>

#define CSH_RL_BUFSIZE 1024
#define CSH_TOK_BUFSIZE 64
#define CSH_TOK_DELIM " \t\r\n\a"

/* char  *see_sh_read_line_old(); */
void   csh_loop(void);
char  *csh_read_line();
char **csh_split_line(char *line);
int    csh_launch(char **args);
int    csh_cd(char **args);
int    csh_help(char **args);
int    csh_quit(char **args);
int    csh_execute(char **args);

/* char *see_sh_read_line_old() { */
/*     size_t bufsize = CSH_RL_BUFSIZE; */
/*     int    position = 0; */
/*     char  *buffer = malloc(sizeof(char) * bufsize); */
/*     int    c; */

/*     if (!buffer) { */
/*         fprintf(stderr, "seesh: allocation error\n"); */
/*         exit(EXIT_FAILURE); */
/*     } */

/*     while (1) { */
/*         c = getchar(); */

/*         // EOF or newline -> replace with null character and return */
/*         if (c == EOF || c == '\n') { */
/*             buffer[position] = 0; */
/*             return buffer; */
/*         } */

/*         buffer[position++] = c; */

/*         // if we exceed buffer size reallocate */
/*         if (position >= bufsize) { */
/*             bufsize += CSH_RL_BUFSIZE; */
/*             buffer = realloc(buffer, bufsize); */

/*             if (!buffer) { */
/*                 buffer[position] = 0; */
/*                 return buffer; */
/*             } */
/*         } */
/*     } */
/* } */

char *builtin_str[] = {"cd", "help", "quit", "exit"};

int (*builtin_func[])(char **) = {&csh_cd, &csh_help, &csh_quit, &csh_quit};

int csh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char *);
}

int main(int argc, char **argv) {
    csh_loop();

    return EXIT_SUCCESS;
}

void csh_loop(void) {
    char  *line;
    char **args;
    int    status;
    int    exit;

    /* int file = open("test.txt", O_RDONLY); */
    /* dup2(file, STDIN_FILENO); */

    do {
        printf("> ");

        line = csh_read_line();
        char **args = csh_split_line(line);
        csh_execute(args);

        free(line);
        free(args);

    } while (exit == 0);
}

int csh_execute(char **args) {
    if (args[0] == NULL) {
        return -1;
    }

    for (int i = 0; i < csh_num_builtins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return csh_launch(args);
}

int csh_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "Expected argument to '%s'\n", args[0]);
        return -1;
    }

    if (chdir(args[1]) == -1) {
        perror("lsh (cd)");
        return -1;
    }

    return 1;
}

int csh_quit(char **args) {
    exit(0);
}

int csh_help(char **args) {
    printf("Cristian Scapin's CSH\n");
    printf("Built in programs:\n");

    for (int i = 0; i < csh_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }
    return 1;
}

char *csh_read_line() {
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

char **csh_split_line(char *line) {
    int    bufsize = CSH_TOK_BUFSIZE;
    int    position = 0;
    char **tokens = malloc(bufsize * sizeof(char));
    char  *token;

    if (tokens == NULL) {
        fprintf(stderr, "seesh: allocation errror\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, CSH_TOK_DELIM);
    while (token != NULL) {
        tokens[position++] = token;

        if (position > bufsize) {
            bufsize += CSH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char));

            if (tokens == NULL) {
                fprintf(stderr, "seesh: allocation errror\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, CSH_TOK_DELIM);
    }

    tokens[position] = NULL;
    return tokens;
}

int csh_launch(char **args) {
    pid_t pid, wait_pid;
    int   status;

    pid = fork();
    if (pid == 0) { // child
        if (execvp(args[0], args) == -1) {
            if (errno == 2) {
                fprintf(stderr, "Command '%s' not found\n", args[0]);
                exit(EXIT_FAILURE);
            }
            perror("see_sh");
            exit(EXIT_FAILURE);
        }
    } else if (pid < 0) { // error on forking
        perror("see_sh");
    } else {
        do {
            wait_pid = waitpid(
                pid, &status, WUNTRACED); // wait fort status change on child
        } while (!WIFEXITED(status) &&
                 !WIFSIGNALED(status)); // exit only if status changed in child
                                        // from termination by return or signal
    }

    return 0;
}
