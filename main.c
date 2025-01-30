#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "navigation.h"
#include "parsing.h"
#include "redirecting.h"
#include "utils.h"
#include "autocompletion.h"

void free_args(char **args)
{
  if (!args)
    return;
  for (int i = 0; args[i] != NULL; i++)
  {
    free(args[i]);
  }
  free(args);
}


int main()
{
  enable_raw_mode();
  init_entries_buffer();

  while (1)
  {

    // Flush after every printf
    setbuf(stdout, NULL);
    printf("$ ");
    char last_input = '\0';
    // Wait for user input
    char input[1024];
    memset(input, 0, sizeof(input));
    // fgets(input, 1024, stdin);
    int idx = 0;
    char ch;
    int printed = 0;
    int tab_presses = 0;
    int suggested = 0;
   

    while (1)
    {
      ch = getchar();
      last_input =ch;
      if (ch == '\n')
      {
        break;
      }
      else if (ch == '\t')
      {
  
        tab_presses++;
        
      



        if (tab_presses == 1  )
        {
          suggested = autocompelte(input, &idx); 
          if(suggested ==1 )tab_presses =0;
        }

        else if ( (tab_presses == 2) )
        {

          // print the buffer with 2 spaces
            print_entries_buffer(input);
            tab_presses = 0;
              

        }
      }

      else
      {
        if (idx < sizeof(input) - 1)
        { // print current char to terminal
          input[idx++] = ch;
          printf("%c", ch);
          printed = 1;
        }
      }
    }
    if (printed)
      printf("\n");

    input[idx] = '\0';
    // input[strlen(input) - 1] = '\0';
    char *command = get_command_from_input(input);
    char **args = parseArguments(input);

    save_fd(); // save current file descriptor to restore it if a redirection happened
    int is_redirected = handle_redirections(args);

    if (is_redirected < 0)
    {
      free_args(args);
      continue;
    }

    if (strcmp("exit 0", input) == 0)
      exit(0);

    else if (strncmp(input, "echo", 4) == 0)
    {

      for (int i = 1; args[i] != NULL; i++)
      {
        printf("%s ", args[i]);
      }
      printf("\n");
    }

    // check the type builtin or no
    else if (strncmp(input, "type", 4) == 0)
    {
      type_command(args); // need to be passes args to deals with redirection
    }

    else if (strcmp(command, "pwd") == 0)
    {
      pwd();
    }
    else if (strcmp(command, "cd") == 0)
    {
      cd(input);
    }
    else if (find_path(command) != NULL)
    { // parse the first command , check if it  is exec , if so check for args and pass it all to exec function

      execute(find_path(command), args);
    }
    else
    {

      printf("%s: command not found\n", input);
    }

    if (is_redirected > -1)
    {
      free_args(args);
      reset_fd();
    }
  }

  return 0;
}
