/* Jaineel Upadhyay
   CMSC 421 - 05
   Project 1 : Implementation of a shell program in user space.
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

// char* r_input() : Asks for input, gets an input and returns it. Uses fgetc().
char* r_input(){
  int c = 0;
  size_t len = 20;
  size_t ind = 0;
  char *cmd = NULL;
  char *temp = NULL;
  cmd = malloc(len);
  printf("\n> ");
  while (c != '\n'){
    if(ind == len){
      len = len + len;
      cmd = realloc(cmd, len);
    }
    c = fgetc(stdin);
    if (c != '\n'){
      cmd[ind] = c;
      ind += 1;
    }
  }
  // Check if input is all whitespace
  int check = 0;
  for(int i = 0; i < strlen(cmd); i++){
    if (isspace(cmd[i])){
      check++;
    }
  }
  // Duplicate and free previous string.
  if (ind != 0 && check != strlen(cmd)){
    temp = strndup(cmd, ind);
  }
  free(cmd);
  cmd = NULL;
  return temp;
}

// char** str_tokens(char* command) : Takes char* command and tokenizes it before storing in an array.
// NULL pushed back at the end of array for exec (execvp).
char** str_tokens(char *command){
  char *svPtr = NULL;
  char*token, *delim = " ";
  
  size_t str_spaces = 0;
  // execvp requires NULL, and if spaces = one, therefore there are two tokens (Example : ls -la)
  str_spaces = count_spaces(command);
  char** parsed = NULL;
  parsed = realloc(parsed, sizeof(char *) * (str_spaces + 2));
  size_t index = 0;
  char *temp;
  // strtok_r to tokenize
  for (token = strtok_r(command, delim, &svPtr); token != NULL; token = strtok_r(NULL, delim, &svPtr)){
      temp = strdup(token);
      parsed[index] = temp;
      index += 1;
  }

  parsed[index] = NULL;

  return parsed;
}

// void execute_command(char**) : Executes the command input parsed.
void execute_command(char** parsed){
  pid_t pid;
  
  // Forking a child process
  pid = fork();
  if (pid < 0){
    // Error in forking
    fprintf(stderr, "Fork Failed!\n"); 
  }
  else if (pid == 0){
    // Child Process
    // If error in executing, exit process.
    if (execvp(parsed[0], parsed) < 0){
      perror(parsed[0]);
      exit(-1);
    }
  }
  else{
    // Parent Process
    wait(NULL);
  }
  return;
}

// void set_environment(char**) : Sets an environment variable based on name and value provided by user.
void set_environment(char* name, char* value){
  int success = setenv(name, value, 1);
  if (success == -1){
    fprintf(stderr, "Error: Wrong command arguments\n");
  }

  return;
}

// void get_environment(char**) : Gets an environment variable value based on name.
void get_environment(char* name){
  if (name == NULL || strcmp(name,"") == 0){
    fprintf(stderr, "Please enter the name of the variable\n");
    return;
  }
  char* value = getenv(name);
  if(value != NULL){
    fprintf(stdout,"%s\n", value);
  }
  else {
    fprintf(stdout, " ");
  }
  return;
}

// void memFree(char**, char*, char*, int) : frees memory allocated. Used to free multiple memories at once.
void memFree(char** parsed, char* command, char* temp, int spaces){
  if (parsed != NULL){
    for(int i = 0; i < spaces; i++){
      if (parsed[i] != NULL){
        free(parsed[i]);
        parsed[i] = NULL;
      }
    }
    free(parsed);
    parsed = NULL;
  }
  if (command != NULL){
    free(command);
    command = NULL;
  }
  if (temp != NULL){
    free(temp);
    temp = NULL;
  }
  return;
}

// char* get_name_setenv(char* ) : Get name for setenv()
char* get_name_setenv(char* ct){
  char* name;
        
  int add_i = first_unquoted_space(ct) + 1;
  int end_name = add_i + first_unquoted_space(ct + add_i + 1) + 1;
 
  name = strndup(ct + add_i, end_name - add_i);
  
  return name;
}

// Get name for getenv
char* get_name_getenv(char* ct){
  char* name;

  int add_i = first_unquoted_space(ct) + 1;
  name = strndup(ct + add_i, strlen(ct) - add_i);

  return name;

}

int main(int argc, char* argv[]){
    // Used to stop warnings.
    (void)argv;
    // No arguments allowed when shell ran.
    if (argc >= 2){
        fprintf(stderr, "No command line arguments accepted\n");
        return 1;
    }

    char* command = NULL, *temp = NULL;
    char** parsed = NULL;
    int check = 0;
    // Prompt up when no arguments
    if (argc < 2){
      while(check == 0){
        // While user input is empty
        do{
            temp = r_input();
        } while (temp == NULL);
        // For setenv and getenv names
        char* ct = strdup(temp);
        // Handle escape sequences and quotes         
        command = unescape(temp, stderr);
        int spaces = count_spaces(temp) + 1;
        // Parse string into tokens and store in array. Not unescaped for setenv and getenv.
        parsed = str_tokens(temp);        

        // Check if exit is called and check for proper arguments for exit.
        if (strcmp(parsed[0], "exit") == 0 && spaces <= 3){
          check = 1;
          // No exit code is provided.
          if (parsed[1] == NULL){
            memFree(parsed,command, temp, spaces);
            memFree(NULL, ct, NULL, 0);
            exit(0);
          }
          // Exit code is provided
          else if (parsed[1] != NULL) {
            long int exitcode;
            char *endptr = NULL;

            // Check if exit code input is negative/positively signed and is a digit. int check used as flag.
            for (size_t i = 0; i < strlen(parsed[1]); i++){
              if (i == 0 && (parsed[1][i] == '-' || parsed[1][i] == '+')){
                check = 1;
              }
              else if (!isdigit(parsed[1][i])){
                check = 0;
              }
            }
            // Free and exit with code
            if (check == 1){
              exitcode = strtol(parsed[1], &endptr, 10);
              memFree(parsed,command, temp, spaces);
              memFree(NULL, ct, NULL, 0);
              exit(exitcode);
            }
          }
        }

        // If built-in setenv is called.
        else if (strcmp(parsed[0], "setenv") == 0){          
          if (count_spaces(ct) < 2){
            fprintf(stderr, "Please provide a value for the variable");
          }
          else{
            int add_i = first_unquoted_space(ct) + 1;
            int end_name = add_i + first_unquoted_space(ct + add_i) + 1 ;
            char* value = unescape(ct + end_name, stderr);

            char*name = get_name_setenv(ct);
            set_environment(name, value);
            memFree(NULL, name, value, 0);
          }

        }

        // If built-in getenv is called.
        else if (strcmp(parsed[0], "getenv") == 0){
          int get_spaces;
          if(ct[(strlen(ct) - 1)] == ' '){
            int index = strlen(ct) - 1;
            while(ct[index] == ' '){
              index--;
            }
            index++;
            // To avoid whitespace after the name to not run getenv().
            char* temp_ct = strndup(ct, index);
            free(ct);
            ct = temp_ct;
            ct[index] = '\0';
            
            get_spaces = count_spaces(ct);
          }
          else {
            get_spaces = count_spaces(ct);
          }
          if (get_spaces > 1){
            fprintf(stderr, "Please enter only the name of the variable\n");
          }
          else if (get_spaces < 1){
            fprintf(stderr, "Please enter the name of the variable\n");
          }
          else{
            char*name = get_name_getenv(ct);
            get_environment(name);
            free(name);
          }
        }

        // If none of the above commands are called.
        else{
          memFree(parsed, NULL, NULL, spaces);
          spaces = count_spaces(command) + 2;
          parsed = NULL;
          parsed = str_tokens(command);    
          execute_command(parsed);
        }
        // Free memory
        printf("\n"); 
        memFree(NULL, ct, NULL, 0);
        memFree(parsed,command, temp, spaces);       
      }
    }
    return 0;
}
