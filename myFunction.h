#ifndef MYFUNCTION_H
#define MYFUNCTION_H


/**
 * @brief Prints a welcome message to the user.
 */
void print_welcome(); 

/**
 * @brief Prints the current location, including hostname, username, and current working directory.
 */
void getlocation();

/**
 * @brief Splits a string into an array of arguments.
 * 
 * @param str The input string to split.
 * @return A dynamically allocated array of strings (arguments).
 */
char **splitArgument(char *str);

/**
 * @brief Logs out the user if the input string is "exit".
 * 
 * @param str The input string to check.
 */
void logout(char *str);

/**
 * @brief Changes the current working directory.
 * 
 * @param args An array of arguments where args[1] is the target directory.
 */
void cd(char **args);

/**
 * @brief Copies the content of a source file to a destination file.
 * 
 * @param args An array of arguments where args[1] is the source file and args[2] is the destination file.
 */
void cp(char **args);

/**
 * @brief Deletes a file.
 * 
 * @param str The name of the file to delete.
 */
void delete(char *str);

/**
 * @brief Creates a pipe between two commands.
 * 
 * @param argv1 The first command and its arguments.
 * @param argv2 The second command and its arguments.
 */
void mypipe(char **argv1, char **argv2);

/**
 * @brief Moves or renames a file.
 * 
 * @param args An array of arguments where args[1] is the source file and args[2] is the destination file.
 */
void move(char **args);

/**
 * @brief Appends a string to a file.
 * 
 * @param args An array of arguments where args[1] is the string to append and args[2] is the target file.
 */
void echopend(char **args);

/**
 * @brief Writes a string to a file, replacing its content.
 * 
 * @param args An array of arguments where args[1] is the string to write and args[2] is the target file.
 */
void echowrite(char **args);

/**
 * @brief Reads and prints the content of a file.
 * 
 * @param args An array of arguments where args[1] is the file to read.
 */
void readfile(char **args);

/**
 * @brief Counts and prints the number of lines or words in a file.
 * 
 * @param args An array of arguments where args[1] is the option (-l for lines, -w for words) and args[2] is the file to count.
 */
void wordCount(char **args);

#endif


