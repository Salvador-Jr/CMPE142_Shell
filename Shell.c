#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void retrieveUser()
{
    char* loggedUser = getenv("USER");
    printf("%s:~$ ", loggedUser);

}
void userCurrentDirectory()
{
    char userCurrentCWD[1024];
    getcwd(userCurrentCWD, 1024);
    printf("%s\n", userCurrentCWD);
    retrieveUser();
}
//todo: need to fork before execing becuase the exec exits when returned. This exit ends the shell but it should only end the child.
void userLSfunction()
{
    char path[512];
    char wholename[512];
    char *next_piece;

    snprintf(path, 511, "/usr/bin:/bin:/tmp");
    next_piece = strtok(path, ":");
    snprintf(wholename, 511, "%s/ls", next_piece);
    if (access(wholename, X_OK) == 0) {
        char *argv[] = {wholename, NULL};
        execv(argv[0], argv);
    }

    while (next_piece != NULL) {
        next_piece = strtok(NULL, ":");
        snprintf(wholename, 511, "%s/ls", next_piece);
        if (access(wholename, X_OK) == 0) {

            char *argv[] = {wholename, NULL};
            execv(argv[0], argv);
            break;
        }
    }
}




void readInput()
{
    char *line = NULL;
    size_t linesize = 0;
    ssize_t linelen;
    //todo: we will need to find a better way for comparing input
    while ((linelen = getline(&line, &linesize, stdin)) != -1)
    {
        if (strncmp("exit", line, 4) == 0)
        {// user wants to exit
            printf("user wants to exit \n");
            exit(0);

        }
        else if (strncmp("pwd", line, 3) == 0)
        {
//            printf("user wants to print \n");
            userCurrentDirectory();
        }
        else if (strncmp("ls", line, 2) == 0)
        {
//            printf("user wants to LS \n");
            userLSfunction();
            retrieveUser();
        }
        else
        {
//        fwrite(line, linelen, 1, stdout);
            printf("Command is not supported \n");
            retrieveUser();
        }
        // fork// if child, exec// if parent, wait

    }
    free(line);
    if (ferror(stdin))
        err(1, "getline");

}


int main(int argc, char **argv)
{
    retrieveUser();
    readInput();

}




