#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include "utils.h"
#include "parsing.h"
#include "redirecting.h"

int is_executable(char *Full_Path)
{
  if (access(Full_Path, X_OK) == 0)
    return 1;

  else
    return 0;
}

// get the path
//  check if executable  not with execlp
//  check for agrs
// wait

// refactor by using args instead of input , after cleaning using the redirect clean method

void execute(char *path, char **args)
{

  int pid = fork();
  if (pid < 0)
  {
    perror("fork"); // Fork failed
  }
  else if (pid == 0)
  {
    // Child process: execute the program
    execvp(path, args);

    perror("execvp");
  }
  else
  {

    int status;

    waitpid(pid, &status, 0);
  }
}

// 1- get the env path  --> using getenv
// 2- cut it into  peices --> using strtok
// 3- search the  directories --> reconstruct the path by appending the command to the end of each path
// 4- check if the file is executable or not
// 5- printf the full path if found and executable
// 6- print not found if not found

char *find_path(char *command)
{

  char *env_path = getenv("PATH");
  if (env_path == NULL)
  {
    return NULL;
  }

  char *env_path_copy = strdup(env_path);
  char *dir = strtok(env_path_copy, ":");
  static char Full_Path[2046];

  while (dir != NULL)
  {
    snprintf(Full_Path, sizeof(Full_Path), "%s/%s", dir, command); // reconstruct the full path

    if (is_executable(Full_Path) == 1)
    {
      free(env_path_copy);
      return Full_Path;
    }

    dir = strtok(NULL, ":");
  }
  free(env_path_copy);
  return NULL;
}

void type_command(char **args)
{

  // char *arguments =  input+5;
  // arguments = trim_whiteSpaces(arguments);
  char *builtins[] = {"echo", "type", "exit", "pwd", "cd"};

  for (int i = 1; args[i] != NULL; i++)
  {
    int found = 0;
    for (int j = 0; j < 5; j++)
    {

      if (strcmp(builtins[j], args[i]) == 0)
      {
        printf("%s is a shell builtin\n", args[i]);
        found = 1;
      }
    }

    if (!found)
    {
      char *tmp = find_path(args[i]);
      if (tmp != NULL)
      {
        printf("%s is %s\n", args[i], tmp);
      }

      else
      {
        printf("%s: not found\n", args[i]);
      }
    }
  }
}
