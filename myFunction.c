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

#define PURPLE "\033[1;35m" // צבע סגול
#define LIGHT_PURPLE "\033[1;36m" // צבע סגול בהיר
#define GREEN "\033[1;32m"  // צבע ירוק
#define BLUE "\033[1;34m"   // צבע כחול
#define YELLOW "\033[1;33m" // צבע צהוב
#define RESET "\033[0m"     // איפוס צבע
#define BUFFER_SIZE 1024
#define ARGUMENTS_SIZE 10

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

char **splitArgument(char *str) {
    int count = 0;
    char **arguments = malloc(sizeof(char*) * ARGUMENTS_SIZE);
    if (!arguments) {
        perror("malloc failed");
        return NULL;
    }
    
    char *token = strtok(str, " ");
    while (token != NULL) {
        arguments[count] = strdup(token);
        if (!arguments[count]) {
            perror("strdup failed");
            freeArguments(arguments);
            return NULL;
        }
        count++;
        token = strtok(NULL, " ");
    }
    arguments[count] = NULL;
    return arguments;
}

void freeArguments(char **args) {
    for (int i = 0; args[i] != NULL; i++) {
        free(args[i]);
    }
    free(args);
}

void logout(char *str) {
    // הסרת רווחים מתחילת המחרוזת
    while (*str == ' ') str++;
    
    // בדיקה אם המחרוזת מתחילה ב-exit
    if (strncmp(str, "exit", 4) == 0) {
        str += 4;
        // דילוג על רווחים אחרי "exit"
        while (*str == ' ') str++;
        
        // אם הגענו לסוף המחרוזת, מבצעים יציאה
        if (*str == '\0') {
            printf("%sLogging out...%s\n", BLUE, RESET);
            exit(0);
        }
    }
}

void cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "%scd: missing argument%s\n", YELLOW, RESET);
        return;
    }
    
    char path[PATH_MAX] = "";
    for (int i = 1; args[i] != NULL; i++) {
        strcat(path, args[i]);
        if (args[i + 1] != NULL) {
            strcat(path, " ");
        }
    }
    
    if (chdir(path) != 0) {
        perror("cd failed");
    }
}

void cp(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "%scp: missing source or destination%s\n", YELLOW, RESET);
        return;
    }
    
    int source = open(args[1], O_RDONLY);
    if (source == -1) {
        perror("cp: source file error");
        return;
    }
    
    int dest = open(args[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest == -1) {
        perror("cp: destination file error");
        close(source);
        return;
    }
    
    char buffer[BUFFER_SIZE];
    ssize_t bytes;
    while ((bytes = read(source, buffer, sizeof(buffer))) > 0) {
        if (write(dest, buffer, bytes) != bytes) {
            perror("cp: write error");
            close(source);
            close(dest);
            return;
        }
    }
    
    if (bytes == -1) {
        perror("cp: read error");
    }
    
    close(source);
    close(dest);
}

void delete(char *str) {
    while (*str == ' ') str++;
    if (unlink(str) == 0) {
        printf("%sFile '%s' deleted successfully.%s\n", YELLOW, str, RESET);
    } else {
        perror("delete: file deletion failed");
    }
}

void mypipe(char **argv1, char **argv2) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid1 = fork();
    if (pid1 == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0) {
        // תהליך ראשון - הכותב לצינור
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        execvp(argv1[0], argv1);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid2 = fork();
    if (pid2 == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {
        // תהליך שני - הקורא מהצינור
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);
        execvp(argv2[0], argv2);
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

void move(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "%smove: missing source or destination%s\n", YELLOW, RESET);
        return;
    }
    
    if (rename(args[1], args[2]) != 0) {
        perror("move: file move failed");
    } else {
        printf("%sFile moved successfully from '%s' to '%s'%s\n", YELLOW, args[1], args[2], RESET);
    }
}

void echopend(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "%sechopend: missing string or file%s\n", YELLOW, RESET);
        return;
    }
    
    FILE *file = fopen(args[2], "a");
    if (!file) {
        perror("echopend: file open failed");
        return;
    }
    
    fprintf(file, "%s\n", args[1]);
    fclose(file);
    printf("%sString appended successfully to '%s'%s\n", YELLOW, args[2], RESET);
}

void echowrite(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "%sechowrite: missing string or file%s\n", YELLOW, RESET);
        return;
    }
    
    FILE *file = fopen(args[2], "w");
    if (!file) {
        perror("echowrite: file open failed");
        return;
    }
    
    fprintf(file, "%s\n", args[1]);
    fclose(file);
    printf("%sString written successfully to '%s'%s\n", YELLOW, args[2], RESET);
}

void readfile(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "%sread: missing file path%s\n", YELLOW, RESET);
        return;
    }
    
    FILE *file = fopen(args[1], "r");
    if (!file) {
        perror("read: file open failed");
        return;
    }
    
    char buffer[BUFFER_SIZE];
    while (fgets(buffer, sizeof(buffer), file)) {
        printf("%s", buffer);
    }
    fclose(file);
}

void wordCount(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "%swordCount: missing option or file path%s\n", YELLOW, RESET);
        return;
    }
    
    FILE *file = fopen(args[2], "r");
    if (!file) {
        perror("wordCount: file open failed");
        return;
    }
    
    int count = 0;
    char buffer[BUFFER_SIZE];
    if (strcmp(args[1], "-l") == 0) {
        while (fgets(buffer, sizeof(buffer), file)) {
            count++;
        }
        printf("%sLines: %d%s\n", YELLOW, count, RESET);
    } else if (strcmp(args[1], "-w") == 0) {
        while (fscanf(file, "%s", buffer) == 1) {
            count++;
        }
        printf("%sWords: %d%s\n", YELLOW, count, RESET);
    } else {
        fprintf(stderr, "%swordCount: invalid option. Use -l for lines or -w for words.%s\n", YELLOW, RESET);
    }
    fclose(file);
}