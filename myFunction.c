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