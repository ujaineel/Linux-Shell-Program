Linux Shell Program created by GitHub Classroom

Author: Jaineel Upadhyay

SHELL PROGRAM 

This README.MD will contain descriptions for functions used and the operations of the shell program. Shell program contains brief but appropriate comments to provide some help to understand the shell operations and function descriptions.

Functions: 

char* input() : This function works as the input collector, as in, uses fgetc() and stdin to collect command input from user. Char* cmd is the one that stores the input first. Char* cmd starts by a malloc of 20 bytes size (len = 20). It uses size_t index to correct the length of the input as input is added. If index's value matches with len (index == len), i.e., 20 bytes of malloc are filled, len is incremented with it's own value, and realloc is used to increase the size of cmd. Also, the fgetc and length checking happens in a while loop with a condition that stays true until user presses enter (newline, \n). There is an if statement to collect the newline as it is an unwanted input. After the while loop ends, the function uses an if statement body to check if there is input entered. If input is not blank, char *temp duplicates the string and adds a null-terminator (\0) at the end of the string.* cmd is freed and set to NULL temp (which is the user input after processing) is returned.

char** str_tokens(char* command) : This function tokenizes the command string and stores it in parsed array (char** parsed). Also, reallocated parsed (or malloc'ed for first time) to the output of count_spaces(command) + 2. Since execvp requires NULL at the end of the array, and 1 space means there are two command words (for lack of better term), like ls -la. So, a for loop runs with the output of strtok_r() (which has command as its initial argument) runs until token is NULL, while also incrementing size_t index which is used to put NULL at the end of the array after for loop, after which parsed is returned.

void execute_command(char** parsed) : Forks a process (pid_t pid = fork()) and executes the command parsed in str_tokens(). If condition checks for success/failure of forking a new process, else if condition checks for whether the process running right now is a child process, and if it is then run execvp with the required arguments from parsed whilst also checking for success/failure of execvp and printing an error if execvp fails and exits the process. Else condition impalies that current process is parent process and it waits till child process finishes.

void set_environment(char* name, char* value) : Sets up the environment variable with the variable name and value of the variable. Use setenv() function with arguments name and value to create the environment variable. If setenv() returns -1, that means setting environment failed and prints to stderr before returning.

void get_environment(char* name) : Checks for an environment variable with the name of char* name. Just to stay on safe side, added an if condition that returns if name is null or blankspace (chances are very, very rare). Fills char* value with the returned value of getenv(). If value is null, it prints just a blankspace. If value is not null, then it prints the value returned by the getenv() function.

void memFree(char** parsed, char* command, char* temp, int spaces) : Frees memory passed to the function through pointers (or NULL pointers). Checks if the passed pointers are not null, before freeing them. Uses argument *spaces* to run a for loop in parsed, freeing all the malloc'ed memory of the char pointers inside parsed, before freeing parsed itself. 

char* get_name_setenv(char* ct) : Finds the name of the environment variable to be used for setenv() function. char* ct is a duplicate of char* temp (in int main()) with the purpose of its use in getenv() and setenv(). Here, the first and the second unquoted spaces are found (add_i and end_name) to identify the start and end of the environment name. ct is offsetted to match the start of the name char byte, and then char* name duplicates ct until the end of the second unquoted space. char* name is returned then.   

char* get_name_getenv(char* ct) : Gets the name of the environment variable to be used for getenv() function. char* ct is a duplicate of char* temp (in int main()) with the purpose of its use in getenv() and setenv(). Here, the first unquoted space (add_i) is found using first_unqouted_space (utils.c) and then char* name duplicates it with strndup which starting from an offsetted location of ct (containing first byte of name) to the end of string (strlen(ct) - add_i). It returns name then. 



HOW THE SHELL WORKS (In code order) : 

- Starts with (void)argv; Since int main can only have either zero or two arguments, (void)argv is used to stop the appearance of the warnings of no use of argv. (void)argv does not affect the shell operations.

- Checks if any argument is given when running simple_shell, and returning 1 and printing to stderr about it if another argument is found.  (Requirement 2 complete)

- If no other argument is provided, then the shell starts an infinite while loop (exits the shell process when exit called) which prompts the user to enter command.     (Requirement 1 complete)

- A do-while is used to check if user enters blankspace or just whitespace, with no other characters. If only blankspace or whitespace, then user is prompted again.

- char* ct (used for getenv() and setenv()) is duplicated with temp (user prompt input).

- int spaces is used for memFree later on. Also, not used in str_tokens, except created a function-scoped integer spaces.

- str_tokens() takes the temp and tokenizes it according to the above function description.

- First, check if exit is the command passed by user.           (If condition, strcmp(parsed[0],"exit"))          (Requirement 7 complete)
- If it is, then check if user passes an exit code string. If there is no exitcode string, then free all allocated memory and exit with exit code 0.
- If there is an exitcode string, then first check if it is a negative number and check if there are no non-digits (other than at start (-,+)) as required.
- If there is a non-digit other than specified inside the for loop - if condition, then change check value (used as flag) to 0 and exit will not run, instead moving ahead.
- If all are digits or required number of characters are digits (negative numbers), then convert exit code string with strtol and exit with the converted exit code value.

- Second, if exit is not entered then check for setenv as the user command. (Else if condition, strcmp(parsed[0],"setenv")).    (Requirement 6 complete)
- If yes, then check if no value is provided. If no value provided, print an error to stderr and move out of else if condition.
- Else, get the value of the environment variable, and use get_name_setenv() to get the name of the variable. Pass it to set_environment() and free memory of name and value (which are duplicated, hence allocated) before moving out of the block of code. Also, value is ran through unescape().

- Third, check if getenv is the command user wants to run instead of the other two. (Else if condition , strcmp(parsed[0],"getenv"))  (Requirement 5 complete)
- If yes, then check if there is whitespace at the end of the string instead of other characters. (To ensure better and easier user experience where user can enter space after name and still run the command. Only for getenv(). Only one whitespace after name).
- Remove all the whitespace, and then check for number of arguments passed by user.
- If no whitespace, then simply find the number of arguments passed by user.
- If less arguments passed, print to stderr that less arguments are passed.
- If more arguments passed, print to stderr that more arguments are passed.
- If adequate number of arguments passed, then get the name of the variable and then value of environment variable through get_environment. Free char* name afterwards (duplicated). 

- If none of the commands are being requested to run.     (Else conditon)       (Requirement 4 complete)
- Free char** parsed as it had parsed values of command input not unescaped. 
- Change spaces to count_spaces(command) + 2; (For memFree and also for reasons as suggested in str_tokens())
- Store tokens of unescaped command string in char** parsed.
- Execute the command using execute_command().

- Since all possible command patterns are done, free all the allocated memory that hasn't been freed previously in main() or other functions. 
(Requirement 8 complete)

Hence, Shell programs runs smoothly, without any memory leaks, and fulfills all requirements as specified.
