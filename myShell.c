#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "myFunction.h"
#include "myShell.h"

#define MAX_INPUT 1024 // גודל מקסימלי של הקלט

int main() {
    print_welcome();  

    char input[MAX_INPUT]; // משתנה לאחסון הקלט
    char **args;
    
    while (1) { // לולאה אינסופית המדמה shell
        getlocation(); // מציג את הנתיב הנוכחי כמו ב-shell
        printf("$ "); // מציג את הסימן המייצג קלט ב-shell
        
        if (fgets(input, MAX_INPUT, stdin) == NULL) { // קורא קלט מהמשתמש
            printf("\n");
            break; // יציאה אם אין קלט
        }
        
        input[strcspn(input, "\n")] = 0; // מסיר את התו \n מהקלט
        if (strlen(input) == 0) continue; // אם הקלט ריק, ממשיכים ללולאה הבאה
        
        logout(input); // בודק אם המשתמש רוצה לצאת
        
        args = splitArgument(input); // מפצל את הקלט למערך של מחרוזות
        if (args == NULL) continue;
        
        execute_command(args); // מפעיל את הפונקציה המתאימה לפי הקלט
        
        freeArguments(args); // משחרר את הזיכרון שהוקצה למערך הפקודות
    }
    return 0;
}

void execute_command(char **args) {
    // בודק אם יש צינור (|) בפקודה
    int i = 0;
    while (args[i] != NULL) {
        if (strcmp(args[i], "|") == 0) {
            // מצאנו צינור, צריך לפצל את הפקודה לשני חלקים
            args[i] = NULL; // מחלק את המערך לשני חלקים
            char **cmd1 = args;     // החלק הראשון של הפקודה
            char **cmd2 = &args[i + 1]; // החלק השני אחרי הצינור

            mypipe(cmd1, cmd2); // מפעיל את הצינור
            return;
        }
        i++;
    }

    // Check for output redirection (>)
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], ">") == 0) {
            execute_command_with_redirection(args);
            return;
        }
    }

    // אם אין צינור, מריץ את הפקודה כרגיל
    if (strcmp(args[0], "cd") == 0) {
        cd(args);
    } else if (strcmp(args[0], "cp") == 0) {
        cp(args);
    } else if (strcmp(args[0], "delete") == 0) {
        delete(args[1]);
    } else if (strcmp(args[0], "move") == 0) {
        move(args);
    } else if (strcmp(args[0], "echo") == 0) {
        echo(args);
    } else if (strcmp(args[0], "read") == 0) {
        readfile(args);
    } else if (strcmp(args[0], "wordCount") == 0) {
        wordCount(args);
    } else {
        pid_t pid = fork();
        if (pid == 0) {
            execvp(args[0], args);
            perror("Command execution failed");
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            wait(NULL);
        } else {
            perror("fork failed");
        }
    }
}