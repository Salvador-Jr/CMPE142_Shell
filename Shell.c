#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
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
    char *lastToken;
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
    else if (strncmp("cd", token, 2) == 0)
    {
//        printf("DB: CDDDDDD\n");
        while (token != NULL) {
            //                printf("DB: Token%i %s,",index, token);
            linePassed[index] = token;
            index++;
            token = strtok(NULL, "/");
        }
        linePassed[index] = NULL;
//        printf (" DB: the last one is:%s", linePassed[index -1]);
        lastToken = linePassed[index -1];
        int len = strlen(lastToken);
        int minOne = len -1;
//        printf("DB: size is: %i\n", len);
        for (int x =0; x <=minOne; x++)
        {
            lastToken[x] = lastToken[x];
        }
        lastToken[minOne] = '\0';
//        printf("DB: lastToken is %s\n", lastToken);
        linePassed[index -1] = lastToken;
        return linePassed;
    }
    else
    {
    while (token != NULL) {
//                printf("DB: Token%i %s,",index, token);
        linePassed[index] = token;
        index++;
        token = strtok(NULL, "/");
        if (token  == NULL)
        {
            //            printf("DB:WE ARE AT THE NULL\n");
        }
    }
    linePassed[index] = NULL;
//    printf ("the last one is:%s", linePassed[index -1]);
        lastToken = linePassed[index -1];
        
    return linePassed;
    }
}

void redir(char *cmd, char **file)
{
    int fd, fds[2], count;
    char x;
    pid_t pid;
    pipe(fds);

        if (fork() == 0)
    {
        //stdin redirection
        fd = open(file[0], O_RDWR, 0777);
        dup2(fds[0], 0);
        close(fds[1]);

        //stdout redirection
        while ((count = read(0, &x, 1)) > 0)
        write (fd, &x, 1);  
        exit (0);

        execvp((const char*)cmd[0], (char *const*) cmd);
    }
}

int whitespacecounter(char *input, int length)  //for whitespaces in an input string
{
    int i = 0;
    int counter = 0;
    for (i = 0; i < length; i++)
    {
        if (input[i] == ' ')
        {
            counter++;
        }
    }
        return counter;
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
    int length;
    char error_message[30] = "An error has occurred\n";
    printShell();
    while ((linelen = getline(&line, &linesize, stdin)) != -1)
    {
        parsedLine = readLine(line);
        if (strncmp("cd", parsedLine[0],2) == 0)
        {
            if(parsedLine[1] == NULL)
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
                printShell();
            }
            else if (parsedLine[1] !=NULL)
            {
                path = parsedLine[1];
                for(int i =1; i < 100; i++)
                {
                    if(parsedLine[i] !=NULL) {
//                        printf("DB:trying to cd on:%s.\n",parsedLine[i]);
                        chdir(parsedLine[i]);
                    }
                    if(parsedLine[i] ==NULL) {
//                        printf("DB:going to exit loop\n");
                        i = 99;
                    }
                }
//                printf("DB:We will now CD %s",path);
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
                path = parsedLine[1];
                printf("%s",path);
                printShell();
            }
        }
        else  if (strcmp("pwd\n", parsedLine[0]) == 0)
        {
//            printf("DB:user wants to print \n");
            userCurrentDirectory();
        }
        else if (strncmp("&", parsedLine[0],1) == 0)    // test 16
        {
           printShell();
        }
        else if (strcmp("ls\n", parsedLine[0]) == 0)

        
        else if (strncmp("", parsedLine[0],0) == 0)    // test 21 (empty command)
        {
           printShell();
        }
        
          else if (strncmp("  ", parsedLine[0],2) == 0)    // test 15 (white space)
        {
           whitespacecounter(line,length); 
           printShell();
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
        else if (strcmp(parsedLine[0], "exit\n") == 0) {
            exit(0);
        }
        else{
            write(STDERR_FILENO, error_message, strlen(error_message));
            printShell();
        }
    }
    free(line);
    free(parsedLine);
    return 0;
}
