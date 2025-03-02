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

// פונקציה שמבצעת את הפקודה המתאימה
void execute_command(char **args) {
    if (strcmp(args[0], "cd") == 0) { // אם הפקודה היא cd
        cd(args);
    } else if (strcmp(args[0], "cp") == 0) { // אם הפקודה היא cp
        cp(args);
    } else if (strcmp(args[0], "delete") == 0) { // אם הפקודה היא delete
        delete(args[1]);
    } else if (strcmp(args[0], "move") == 0) { // אם הפקודה היא move
        move(args);
    } else if (strcmp(args[0], "echopend") == 0) { // אם הפקודה היא echopend
        echopend(args);
    } else if (strcmp(args[0], "echowrite") == 0) { // אם הפקודה היא echowrite
        echowrite(args);
    } else if (strcmp(args[0], "read") == 0) { // אם הפקודה היא read
        readfile(args);
    } else if (strcmp(args[0], "wordCount") == 0) { // אם הפקודה היא wordCount
        wordCount(args);
    } else {
        pid_t pid = fork(); // יוצר תהליך בן
        if (pid == 0) { // אם זהו תהליך הבן
            execvp(args[0], args); // מפעיל את הפקודה
            perror("Command execution failed"); // הודעת שגיאה במקרה של כשלון
            exit(EXIT_FAILURE);
        } else if (pid > 0) { // תהליך האב מחכה לסיום התהליך הבן
            wait(NULL);
        } else {
            perror("fork failed"); // הודעת שגיאה אם fork נכשל
        }
    }
}