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