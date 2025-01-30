#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "navigation.h"
#include "parsing.h"
#include "utils.h"

int original_stdout;
int original_stderr;
void save_fd() {
    original_stdout = dup(STDOUT_FILENO);  // Save the original stdout
    if (original_stdout < 0) {
        perror("Failed to save stdout");
        exit(1);
    }
    original_stderr = dup(STDERR_FILENO);  // Save the original stderr
    if (original_stderr < 0) {
        perror("Failed to save stderr");
        exit(1);
    }
}

void reset_fd() {
    if (dup2(original_stdout, STDOUT_FILENO) < 0) {
        perror("Failed to restore stdout");
        exit(1);
    }
     if (dup2(original_stderr, STDERR_FILENO) < 0) {
        perror("Failed to restore stderr");
        exit(1);
    }
    //close(STDERR_FILENO);
    close(original_stdout);  // Close after restoring
    close(original_stdout);  // Close after restoring

}

static void remove_redirection_tokens(char **args, int start)
{
  while (args[start + 2] != NULL)
  {
    args[start] = args[start + 2];
    start++;
  }
  args[start] = NULL;
  args[start + 1] = NULL;
}

int handle_redirections(char **args)
{
  for (int i = 0; args[i] != NULL; i++)
  {
    if (strcmp(args[i], ">") == 0 || strcmp(args[i], "1>") == 0 ||
        strcmp(args[i], ">>") == 0 || strcmp(args[i], "2>") == 0 ||
        strcmp(args[i], "1>>") == 0 || strcmp(args[i], "2>>")==0)
    {

      int target_fd;
      int flags = 0;
      mode_t mode = 0777;
      char *file_name = args[i + 1];

      if (strcmp(args[i], ">") == 0 || strcmp(args[i], "1>") == 0)
      {
        target_fd = STDOUT_FILENO;
        flags = O_CREAT | O_WRONLY | O_TRUNC;
      }

      else if (strcmp(args[i], ">>") == 0 || strcmp(args[i], "1>>") == 0)
      { // append stdout
        target_fd = STDOUT_FILENO;
        flags = O_CREAT | O_WRONLY | O_APPEND;
      }

      else if (strcmp(args[i], "2>") == 0)
      { // redirecr stderr
        target_fd = STDERR_FILENO;
        flags = O_CREAT | O_WRONLY | O_TRUNC;
      }
      else if (strcmp(args[i], "2>>") == 0)
      { // append stderr
        target_fd = STDERR_FILENO;
        flags = O_CREAT | O_WRONLY | O_APPEND;
      }

      else
      {
        fprintf(stderr, "Unsupported redirection operator: %s\n", args[i]);
        return -1;
      }

      int fd = open(file_name, flags, mode);
      if (fd < 0)
      {
        perror("open");
        return -1;
      }

      if (dup2(fd, target_fd) < 0)
      {
        perror("dup2");
        close(fd);
        return -1;
      }

      close(fd);

      // Remove the redirection operator and filename from args
      remove_redirection_tokens(args, i);
      i-=1;
    }
  }
  return 0;
}


