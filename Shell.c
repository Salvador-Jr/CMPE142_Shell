#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#define Byte 8

//DB stands for Debug print statment. Will need to delete before submitting
void printShell()
{
    printf("Shell> ");
}
void userCurrentDirectory()
{
    char userCurrentCWD[1024];
    getcwd(userCurrentCWD, 1024);
    printf("%s\n", userCurrentCWD);
    printShell();
}
char **readLine(char *Line)
{
    char **linePassed = malloc(Byte * sizeof(char *));
    char *token;
    char *path = "path";
    char **updatedPath = malloc(Byte * sizeof(char *));
    int index = 0;
    char *NullTerminat = "\0";
    
    token = strtok(Line, " ");
    
    if (strncmp("path", token, 4) == 0)
    {
        while(token != NULL)
        {
            updatedPath[index] = token;
            index++;
            token = strtok(NULL, " ");
        }
        updatedPath[index] = NULL;
        return updatedPath;
    }
    else
    {
    while (token != NULL) {
        //        printf("DB: Token%i %s,",index, token);
        linePassed[index] = token;
        index++;
        token = strtok(NULL, " ");
        if (token  == NULL)
        {
            //            printf("DB:WE ARE AT THE NULL\n");
        }
    }
    linePassed[index] = NULL;
    return linePassed;
    }
}

int main() {
    char **parsedLine;
    char *line = NULL;
    char *path = NULL;
    char *NullTerminated = "\0";
    size_t linesize = 0;
    ssize_t linelen;
    pid_t child_pid;
    int stat_loc;
    char error_message[30] = "An error has occurred\n";
    printShell();
    while ((linelen = getline(&line, &linesize, stdin)) != -1)
    {
        parsedLine = readLine(line);
        //        printf(" DB: your commands: %s, %s\n", parsedLine[0], parsedLine[1]);
        //Keep this in case you need to exit but modified the "exit" if-statment. Delete before submititng
        if (strncmp("leave", parsedLine[0], 4) == 0)
        {// user wants to exit
            printf("DB:user wants to exit \n");
            exit(0);
        }
        else if (strncmp("cd", parsedLine[0],2) == 0)
        {
            if(parsedLine[1] == NULL)
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
                printShell();
            }
            else if (parsedLine[1] !=NULL)
            {
                path = parsedLine[1];
                printf("DB:We will now CD %s",path);
                printShell();
            }
        }
        else if (strncmp("path", parsedLine[0],4) == 0)
        {
            if(parsedLine[1] == NULL)
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
                printShell();
            }
            else if (parsedLine[1] !=NULL)
            {
                printf("DB:your selected path is %s",parsedLine[1] );
                printShell();
            }
        }
        else  if (strncmp("pwd", line, 3) == 0)
        {
            printf("DB:user wants to print \n");
            userCurrentDirectory();
        }
        
        else if (strncmp("ls", parsedLine[0],2) == 0)
        {
//            printf("DB:list \n");
            child_pid = fork();
            if (child_pid < 0) {
                write(STDERR_FILENO, error_message, strlen(error_message));
                exit(1);
            }
            if (child_pid == 0) {
                /* Never returns if the call is successful */
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
            } else {
                waitpid(child_pid, &stat_loc, WUNTRACED);
            }
            printShell();
        }
        //This exit needs a space at the end to exit. When we parse through it saves the null terminator and therefore will not reutrn. need to debug.
        else if (strcmp(parsedLine[0], "exit") == 0) {
            exit(0);
        }
        else{
            write(STDERR_FILENO, error_message, strlen(error_message));
            printShell();
        }
        
        //        free(line);
        //        free(parsedLine);
    }
    
    return 0;
}

