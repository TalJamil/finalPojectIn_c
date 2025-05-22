#ifndef MYFUNCTION_H
#define MYFUNCTION_H

/**
 * @brief Copies the content of a source file to a destination file.
 * @param arguments Array of arguments (arguments[1] is the source, arguments[2] is the destination).
 */
void cp(char **arguments);
/**
 * @brief Prints a welcome message to the user.
 */
void print_welcome();

/**
 * @brief Prints the current location (username, hostname, and current directory).
 */
void getlocation();

/**
 * @brief Reads a line of input from the user.
 * @return Pointer to a dynamically allocated string containing the input.
 */
char *inputFromUser();

/**
 * @brief Changes the current working directory.
 * @param arguments Array of arguments (arguments[1] is the target directory).
 */
void cd(char **arguments);



/**
 * @brief Splits a string into an array of arguments.
 * @param arr The input string to split.
 * @return A dynamically allocated array of strings (arguments).
 */
char **splitArguments(char *input);

/**
 * @brief Exits the shell program.
 */
void logout();

/**
 * @brief Executes a command using fork and execvp.
 * @param arguments Array of arguments (first is the command).
 */
void systemCall(char **arguments);

/**
 * @brief Executes up to three commands connected by pipes.
 * @param argv1 Arguments for the first command.
 * @param argv2 Arguments for the second command.
 * @param argv3 Arguments for the third command (can be NULL).
 */
void myPipe(char **argv1, char **argv2, char **argv3);

/**
 * @brief Returns the index of the first pipe symbol in the arguments array.
 * @param arguments Array of arguments.
 * @return Index of the pipe symbol, or 0 if not found.
 */
int getPipe(char **arguments);

/**
 * @brief Deletes a file.
 * @param arguments Array of arguments (arguments[1] is the file to delete).
 */
void del(char **arguments);

/**
 * @brief Moves or renames a file.
 * @param arguments Array of arguments (arguments[1] is the source, arguments[2] is the destination).
 */
void move(char **arguments);

/**
 * @brief Prints a string or writes/appends it to a file.
 * @param arguments Array of arguments (supports echo, echo >, echo >>).
 */
void echoFile(char **arguments);


/**
 * @brief Counts the number of words, lines, and characters in a file.
 * @param arguments Array of arguments (arguments[1] is the file to count).
 */
void wordCount(char **arguments);


/**
 * @brief Reads and prints the content of a file to the screen.
 * @param arguments Array of arguments (arguments[1] is the file to read).
 */
void readfile(char **arguments);

#endif