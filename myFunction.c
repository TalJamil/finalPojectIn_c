#include "myFunction.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <linux/limits.h>
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

// פונקציה שמדפיסה הודעת ברוכים הבאים
void print_welcome() {
    printf("%s", BLUE); // שינוי צבע הטקסט לכחול
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
    printf("%s", RESET); // איפוס צבע הטקסט
}

// פונקציה שמדפיסה את המיקום הנוכחי של המשתמש
void getlocation() {
    char cwd[PATH_MAX]; // משתנה לאחסון הנתיב הנוכחי
    char hostname[sysconf(_SC_HOST_NAME_MAX)]; // משתנה לאחסון שם המחשב
    struct passwd *pw; // מבנה לאחסון מידע על המשתמש
    
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
    int count = 0; // מונה את מספר המילים
    char **arguments = malloc(sizeof(char*) * ARGUMENTS_SIZE); // מקצה זיכרון למערך המחרוזות
    if (!arguments) {
        perror("malloc failed");
        return NULL;
    }

    while (*str) {
        // דילוג על רווחים
        while (*str == ' ') str++;

        char *start;
        if (*str == '\'') {
            // אם המחרוזת מתחילה בגרשיים
            str++;
            start = str;
            while (*str && *str != '\'') str++;
            if (*str == '\'') {
                *str = '\0';
                str++;
            }
        } else {
            // אם המחרוזת לא מתחילה בגרשיים
            start = str;
            while (*str && *str != ' ') str++;
            if (*str) {
                *str = '\0';
                str++;
            }
        }

        arguments[count] = strdup(start); // מעתיק את המילה למערך
        if (!arguments[count]) {
            perror("strdup failed");
            freeArguments(arguments);
            return NULL;
        }
        count++;
    }
    arguments[count] = NULL; // מסמן את סוף המערך
    return arguments;
}

// פונקציה שמשחררת את הזיכרון שהוקצה למערך המחרוזות
void freeArguments(char **args) {
    for (int i = 0; args[i] != NULL; i++) {
        free(args[i]); // משחרר כל מחרוזת במערך
    }
    free(args); // משחרר את המערך עצמו
}

// פונקציה שמבצעת יציאה מהתוכנית אם המשתמש הקליד exit
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

// פונקציה שמשנה את הספרייה הנוכחית
void cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "%scd: missing argument%s\n", YELLOW, RESET);
        return;
    }
    
    char path[PATH_MAX] = ""; // משתנה לאחסון הנתיב
    for (int i = 1; args[i] != NULL; i++) {
        strcat(path, args[i]); // מחבר את כל המילים לנתיב אחד
        if (args[i + 1] != NULL) {
            strcat(path, " ");
        }
    }
    
    if (chdir(path) != 0) { // משנה את הספרייה הנוכחית
        perror("cd failed");
    }
}

// פונקציה שמעתיקה קובץ ממקום אחד למקום אחר
void cp(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "%scp: missing source or destination%s\n", YELLOW, RESET);
        return;
    }
    
    int source = open(args[1], O_RDONLY); // פותח את קובץ המקור לקריאה
    if (source == -1) {
        perror("cp: source file error");
        return;
    }
    
    int dest = open(args[2], O_WRONLY | O_CREAT | O_TRUNC, 0644); // פותח את קובץ היעד לכתיבה
    if (dest == -1) {
        perror("cp: destination file error");
        close(source);
        return;
    }
    
    char buffer[BUFFER_SIZE]; // משתנה לאחסון הנתונים המועתקים
    ssize_t bytes;
    while ((bytes = read(source, buffer, sizeof(buffer))) > 0) { // קורא את הנתונים מקובץ המקור
        if (write(dest, buffer, bytes) != bytes) { // כותב את הנתונים לקובץ היעד
            perror("cp: write error");
            close(source);
            close(dest);
            return;
        }
    }
    
    if (bytes == -1) {
        perror("cp: read error");
    }
    
    close(source); // סוגר את קובץ המקור
    close(dest); // סוגר את קובץ היעד
}

// פונקציה שמוחקת קובץ
void delete(char *str) {
    // הסרת רווחים מתחילת המחרוזת
    while (*str == ' ') str++;
    
    // בדיקה אם המחרוזת ריקה
    if (*str == '\0') {
        fprintf(stderr, "%sdelete: missing file path%s\n", YELLOW, RESET);
        return;
    }
    
    // ניסיון למחוק את הקובץ
    if (unlink(str) == 0) {
        printf("%sFile '%s' deleted successfully.%s\n", YELLOW, str, RESET);
    } else {
        perror("delete: file deletion failed");
    }
}

// פונקציה שמבצעת צינור בין שתי פקודות
void mypipe(char **argv1, char **argv2) {
    int pipefd[2]; // משתנה לאחסון תיאורי הקבצים של הצינור
    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid1 = fork(); // יוצר תהליך בן ראשון
    if (pid1 == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0) {
        // תהליך ראשון - הכותב לצינור
        close(pipefd[0]); // סוגר את הקצה הקורא של הצינור
        dup2(pipefd[1], STDOUT_FILENO); // מחבר את הקצה הכותב של הצינור ל-stdout
        close(pipefd[1]); // סוגר את הקצה הכותב של הצינור
        execvp(argv1[0], argv1); // מפעיל את הפקודה הראשונה
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid2 = fork(); // יוצר תהליך בן שני
    if (pid2 == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {
        // תהליך שני - הקורא מהצינור
        close(pipefd[1]); // סוגר את הקצה הכותב של הצינור
        dup2(pipefd[0], STDIN_FILENO); // מחבר את הקצה הקורא של הצינור ל-stdin
        close(pipefd[0]); // סוגר את הקצה הקורא של הצינור
        execvp(argv2[0], argv2); // מפעיל את הפקודה השנייה
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }

    close(pipefd[0]); // סוגר את הקצה הקורא של הצינור בתהליך האב
    close(pipefd[1]); // סוגר את הקצה הכותב של הצינור בתהליך האב
    waitpid(pid1, NULL, 0); // מחכה לסיום התהליך הראשון
    waitpid(pid2, NULL, 0); // מחכה לסיום התהליך השני
}

// פונקציה שמעבירה או משנה שם של קובץ
void move(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "%smove: missing source or destination%s\n", YELLOW, RESET);
        return;
    }
    
    if (rename(args[1], args[2]) != 0) { // משנה את שם הקובץ או מעביר אותו
        perror("move: file move failed");
    } else {
        printf("%sFile moved successfully from '%s' to '%s'%s\n", YELLOW, args[1], args[2], RESET);
    }
}

// פונקציה שמבצעת את פקודת echo
void echo(char **args) {
    for (int i = 1; args[i] != NULL; i++) {
        printf("%s ", args[i]);
    }
    printf("\n");
}

// פונקציה שקוראת ומציגה את תוכן הקובץ
void readfile(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "%sread: missing file path%s\n", YELLOW, RESET);
        return;
    }
    
    FILE *file = fopen(args[1], "r"); // פותח את הקובץ לקריאה
    if (!file) {
        perror("read: file open failed");
        return;
    }
    
    char buffer[BUFFER_SIZE]; // משתנה לאחסון הנתונים הנקראים
    while (fgets(buffer, sizeof(buffer), file)) { // קורא את הנתונים מהקובץ
        printf("%s", buffer); // מציג את הנתונים
    }
    fclose(file); // סוגר את הקובץ
}

// פונקציה שסופרת ומציגה את מספר השורות או המילים בקובץ
void wordCount(char **args) {
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "%swordCount: missing option or file path%s\n", YELLOW, RESET);
        return;
    }
    
    FILE *file = fopen(args[2], "r"); // פותח את הקובץ לקריאה
    if (!file) {
        perror("wordCount: file open failed");
        return;
    }
    
    int count = 0; // מונה את מספר השורות או המילים
    char buffer[BUFFER_SIZE]; // משתנה לאחסון הנתונים הנקראים
    if (strcmp(args[1], "-l") == 0) { // אם האופציה היא -l (ספירת שורות)
        while (fgets(buffer, sizeof(buffer), file)) {
            count++;
        }
        printf("%sLines: %d%s\n", YELLOW, count, RESET);
    } else if (strcmp(args[1], "-w") == 0) { // אם האופציה היא -w (ספירת מילים)
        while (fscanf(file, "%s", buffer) == 1) {
            count++;
        }
        printf("%sWords: %d%s\n", YELLOW, count, RESET);
    } else {
        fprintf(stderr, "%swordCount: invalid option. Use -l for lines or -w for words.%s\n", YELLOW, RESET);
    }
    fclose(file); // סוגר את הקובץ
}

// פונקציה שמבצעת פקודה עם הפניה מחדש של פלט
void execute_command_with_redirection(char **args) {
    int fd = -1;
    pid_t pid;
    int background = 0;

    // Check for background process
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "&") == 0) {
            background = 1;
            args[i] = NULL;
            break;
        }
    }

    // Check for output redirection
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], ">") == 0) {
            args[i] = NULL;
            fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("open");
                return;
            }
            break;
        }
    }

    pid = fork();
    if (pid == 0) {
        // Child process
        if (fd >= 0) {
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        execvp(args[0], args);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Fork failed
        perror("fork");
    } else {
        // Parent process
        if (!background) {
            waitpid(pid, NULL, 0);
        }
    }
}