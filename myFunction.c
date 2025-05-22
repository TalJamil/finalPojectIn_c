#include "myFunction.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/utsname.h>
#include <pwd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <libgen.h>
#include <ftw.h>

#define PURPLE "\033[1;35m" // צבע סגול
#define LIGHT_PURPLE "\033[1;36m" // צבע סגול בהיר
#define GREEN "\033[1;32m"  // צבע ירוק
#define BLUE "\033[1;34m"   // צבע כחול
#define RESET "\033[0m"     // איפוס צבע
#define BUFFER_SIZE 1024
#define ARGUMENTS_SIZE 10


void cp(char **arguments){
    int size = 0;
    while (arguments[size] != NULL) {
        size++;
    }

    if(size > 3){
        perror("too many arguments");
        return;
    }
    if(size < 3){
        perror("missing arguments");
        return;
    }

    FILE *fptr1, *fptr2;
    int c;

    // אם קובץ המקור לא קיים, צור אותו ריק
    fptr1 = fopen(arguments[1], "r");
    if (fptr1 == NULL) {
        fptr1 = fopen(arguments[1], "w");
        if (fptr1 == NULL) {
            printf("Cannot create source file %s\n", arguments[1]);
            return;
        }
        fclose(fptr1);
        fptr1 = fopen(arguments[1], "r");
    }

    // בדוק אם היעד הוא תיקיה
    struct stat st;
    char dest_path[PATH_MAX];
    if (stat(arguments[2], &st) == 0 && S_ISDIR(st.st_mode)) {
        // צור נתיב יעד: <תיקיה>/<שם קובץ>
        snprintf(dest_path, sizeof(dest_path), "%s/%s", arguments[2], basename(arguments[1]));
    } else {
        strncpy(dest_path, arguments[2], sizeof(dest_path));
        dest_path[sizeof(dest_path)-1] = '\0';
    }

    fptr2 = fopen(dest_path, "w");
    if (fptr2 == NULL) {
        printf("Cannot open destination file %s\n", dest_path);
        fclose(fptr1);
        return;
    }

    while ((c = fgetc(fptr1)) != EOF) {
        fputc(c, fptr2);
    }

    fclose(fptr1);
    fclose(fptr2);
}
void print_welcome() {
    printf("%s", BLUE);
    printf("=====================================\n");
    printf("             WELCOME!               \n");
    printf("=====================================\n");
    printf("            .-\"\"\"\"\"-.               \n");
    printf("           /          \\             \n");
    printf("          |            |            \n");
    printf("          |,  .-.  .-. |            \n");
    printf("          | )(_o/  \\o_)|            \n");
    printf("          |/     /\\    \\|           \n");
    printf("          (_     ^^     _)          \n");
    printf("           \\__|IIIIII|__/           \n");
    printf("            | \\IIIIII/ |            \n");
    printf("            \\          /            \n");
    printf("             `--------`             \n");
    printf("=====================================\n");
    printf("          welcome to my shell!             \n");
    printf("=====================================\n");
    printf("%s", RESET);
}


void getlocation() {
    char cwd[PATH_MAX];
    char hostname[HOST_NAME_MAX];
    struct passwd *pw;
    
    // קבלת שם המשתמש
    pw = getpwuid(getuid());
    char *username = pw ? pw->pw_name : "Unknown";
    
    // קבלת שם המחשב
    if (gethostname(hostname, sizeof(hostname)) != 0) {
        perror("gethostname failed");
        return;
    }
    
    // קבלת הנתיב הנוכחי
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd failed");
        return;
    }
    
    // הדפסת המידע
    printf("%s%s%s@%s%s%s:%s%s%s\n", GREEN, username, RESET, BLUE, hostname, RESET, LIGHT_PURPLE, cwd, RESET);
}


char *inputFromUser()
{
    char ch;
    int len = 0;
    char *input = (char *)malloc(sizeof(char) * (len + 1));
    *input = '\0';
    while ((ch = getchar()) != '\n')
    {
     *(input + len) = ch;
        input = (char *)realloc(input, (++len + 1));
    }
    *(input + len) = '\0';

    return input;
}


void cd(char **arguments){
    int size = 0;
    while (arguments[size] != NULL) {
        size++;
    }
    
    if(size>2){
        perror("to manny arguments");
    }
    else if(size==1){
        chdir(getenv("HOME"));
    }else{
        chdir(arguments[1]);  
    }

}

 char **splitArguments(char *input) {
    int capacity = 10;
    char **args = malloc(capacity * sizeof(char*));
    int i = 0;
    char *p = input;
    while (*p) {
        while (*p == ' ' || *p == '\t') p++;
        if (!*p) break;

        if (*p == '"' || *p == '\'') {
            char quote = *p++;
            char *start = p;
            while (*p && *p != quote) p++;
            int len = p - start;
            args[i++] = strndup(start, len);
            if (*p) p++;
        } else if (*p == '>' && *(p+1) == '>') {
            args[i++] = strdup(">>");
            p += 2;
        } else if (*p == '>') {
            args[i++] = strdup(">");
            p++;
        } else {
            char *start = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '>') p++;
            int len = p - start;
            // בדוק אם יש >> או > צמודים לסוף המילה
            if (len > 0 && (*(p) == '>' || (*(p) == '>' && *(p+1) == '>'))) {
                // פיצול המילה מהמפעיל
                int word_len = len;
                // בדוק אם יש >> בסוף
                if (word_len >= 2 && start[word_len-2] == '>' && start[word_len-1] == '>') {
                    args[i++] = strndup(start, word_len-2);
                    args[i++] = strdup(">>");
                } else if (word_len >= 1 && start[word_len-1] == '>') {
                    args[i++] = strndup(start, word_len-1);
                    args[i++] = strdup(">");
                } else {
                    args[i++] = strndup(start, word_len);
                }
            } else {
                args[i++] = strndup(start, len);
            }
        }
        if (i >= capacity - 1) {
            capacity *= 2;
            args = realloc(args, capacity * sizeof(char*));
        }
    }
    args[i] = NULL;
    return args;
}
void logout()
{
    puts("logout...");
    exit(EXIT_SUCCESS);
}


void systemCall(char **arguments)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork err");
        return;
    }

    if (pid == 0)
    {

        if (execvp(arguments[0], arguments) == -1)
        {
            exit(EXIT_FAILURE);
        }
    }
}

void myPipe(char **argv1, char **argv2, char **argv3)
{

    int fd1[2], fd2[2];

    if (pipe(fd1) == -1) {
        perror("pipe1 failed");
        exit(1);
    }

    if (argv3 != NULL) {  // create second pipe if argv3 isnt null
        if (pipe(fd2) == -1) {
            perror("pipe2 failed");
            exit(1);
        }
    }

    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("fork1 failed");
        exit(1);
    }
    if (pid1 == 0) {
        close(fd1[0]);  //close fd1 read
        dup2(fd1[1], STDOUT_FILENO);  //redirect output to fd1[1]
        close(fd1[1]);  //close original write

        execvp(argv1[0], argv1);
        perror("execvp1 failed");
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("fork2 failed");
        exit(1);
    }
    if (pid2 == 0) {
        close(fd1[1]);  //close write of fd1
        dup2(fd1[0], STDIN_FILENO);  //read from fd1 [0]
        close(fd1[0]);  // close the original read

        if (argv3 != NULL) { // if i have second pipe
            close(fd2[0]);  // close fd2[0]
            dup2(fd2[1], STDOUT_FILENO);  //redirect the output to fd2[1]
            close(fd2[1]);  //close original write
        }

        execvp(argv2[0], argv2);
        perror("execvp2 failed");
        exit(1);
    }

    //done with first pipe
    close(fd1[0]);
    close(fd1[1]);

    if (argv3 != NULL) { // if second pipe
        pid_t pid3 = fork();
        if (pid3 < 0) {
            perror("fork3 failed");
            exit(1);
        }
        if (pid3 == 0) {
            close(fd2[1]);  //close write of the pipe
            dup2(fd2[0], STDIN_FILENO);  //read from the pipe
            close(fd2[0]);  // close original read

            execvp(argv3[0], argv3);
            perror("execvp3 failed");
            exit(1);
        }
        //close the second pipe
        close(fd2[0]);
        close(fd2[1]);
    }

    wait(NULL);
    wait(NULL);
    if (argv3 != NULL) {
        wait(NULL);
    }
}

int getPipe(char **arguments){
    int i = 0;
    while(arguments[i]  != NULL){
        if(arguments[i][0] == '|'){
            arguments[i] = NULL;
            return i;
        }
        i = i+1;
    }
    return 0;
}

void del(char **arguments)
{
    int size = 0;
    while (arguments[size] != NULL) {
        size++;
    }

    if(size < 2){
        perror("delete needs a path");
        return;
    }
    else if(size > 2){
        perror("too many arguments");
        return;
    }

    struct stat st;
    if (stat(arguments[1], &st) == 0 && S_ISDIR(st.st_mode)) {
        // אם זה תיקיה - מחק עם rm -rf
        char cmd[PATH_MAX + 20];
        snprintf(cmd, sizeof(cmd), "rm -rf \"%s\"", arguments[1]);
        int ret = system(cmd);
        if (ret == 0) {
            printf("Directory deleted successfully.\n");
        } else {
            perror("Error: Unable to delete the directory.");
        }
    } else {
        // קובץ רגיל
        if (remove(arguments[1]) == 0) {
            printf("File deleted successfully.\n");
        } else {
            perror("Error: Unable to delete the file.");
        }
    }
}


void move(char **arguments){
    int size = 0;
    while (arguments[size] != NULL) {
        size++;
    }

    if(size != 3){
        perror("move needs a file source path and destination path");
        return;
    }

    struct stat st;
    char dest_path[PATH_MAX];
    if (stat(arguments[2], &st) == 0 && S_ISDIR(st.st_mode)) {
        // אם היעד הוא תיקיה, הוסף את שם הקובץ לנתיב
        snprintf(dest_path, sizeof(dest_path), "%s/%s", arguments[2], basename(arguments[1]));
    } else {
        strncpy(dest_path, arguments[2], sizeof(dest_path));
        dest_path[sizeof(dest_path)-1] = '\0';
    }

    int status = rename(arguments[1], dest_path);

    if(status != 0 ){
        perror("coulndt move file");
    }
}

void echoFile(char **arguments) {
    int size = 0;
    while (arguments[size] != NULL) size++;

    if(size == 2) {
        puts(arguments[1]);
        return;
    }

    char *dest = NULL;
    if (size == 4) {
        dest = arguments[3];
    } else if (size == 3) {
        dest = arguments[2];
    } else {
        perror("Incorrect number of arguments echoppend needs to be used like <string> >or>> <destination to append/write to>");
        return;
    }

    // --- יצירת תיקיה אם צריך ---
    char dest_copy[PATH_MAX];
    strncpy(dest_copy, dest, PATH_MAX-1);
    dest_copy[PATH_MAX-1] = '\0';
    char *slash = strrchr(dest_copy, '/');
    if (slash) {
        *slash = '\0';
        struct stat st = {0};
        if (stat(dest_copy, &st) == -1) {
            mkdir(dest_copy, 0755);
        }
    }
    // --- סוף יצירת תיקיה ---

    FILE *destFile;
    char *input = arguments[1];

    if(strcmp(arguments[2],">") == 0){
        destFile = fopen(dest, "w");
    }
    else if (strcmp(arguments[2],">>") == 0){
        destFile = fopen(dest, "a");
    }
    else{
        perror("expected > or >> after string but didnt get it");
        return;
    }

    if (destFile == NULL) {
        perror("Error opening file");
        return;
    }

    fprintf(destFile, "%s", input);
    fclose(destFile);
}
void readfile(char **arguments) {
    if (!arguments[1]) {
        printf("No file specified\n");
        return;
    }
    FILE *f = fopen(arguments[1], "r");
    if (!f) {
        // אם לא קיים, צור אותו
        f = fopen(arguments[1], "w");
        if (!f) {
            printf("Cannot create file: %s\n", arguments[1]);
            return;
        }
        printf("File '%s' did not exist and was created (empty).\n", arguments[1]);
        fclose(f);
        return;
    }
    int c;
    while ((c = fgetc(f)) != EOF)
        putchar(c);
    fclose(f);
    printf("\n");

    
}
void wordCount(char **arguments) {
    if (arguments[1] == NULL || arguments[2] == NULL) {
        perror("missing arguments");
        return;
    }
    FILE *f = fopen(arguments[2], "r");
    if (!f) {
        perror("cannot open file");
        return;
    }
    int count = 0;
    if (strcmp(arguments[1], "-w") == 0) {
        char word[256];
        while (fscanf(f, "%255s", word) == 1) count++;
        printf("%d\n", count);
    } else if (strcmp(arguments[1], "-l") == 0) {
        char line[1024];
        while (fgets(line, sizeof(line), f)) count++;
        printf("%d\n", count);
    } else {
        perror("invalid flag");
    }
    fclose(f);
}
