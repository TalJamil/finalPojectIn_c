#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "myFunction.h"
#include "myShell.h"


int main(int argc, char const *argv[])
{
    print_welcome();
    while (1) {
        getlocation();
         
        char *input = inputFromUser();
        char **args = splitArguments(input);
        if (!args || !args[0]) {
            free(input); free(args); continue;
        }
        if (strcmp(args[0], "exit") == 0) logout();
        else if (strcmp(args[0], "cd") == 0) cd(args);
        else if (strcmp(args[0], "cp") == 0) cp(args);
        else if (strcmp(args[0], "mv") == 0) move(args);
        else if (strcmp(args[0], "delete") == 0) del(args);
        else if (strcmp(args[0], "echo") == 0) echoFile(args);
        else if (strcmp(args[0], "read") == 0) readfile(args);
        else if (strcmp(args[0], "wc") == 0) wordCount(args);
        else {
            int pipe1 = getPipe(args);
            if (pipe1)
                myPipe(args, args+pipe1+1, NULL);
            else
                systemCall(args);
            wait(NULL);
        }
        free(input);
        free(args);
    }
    return 0;
}