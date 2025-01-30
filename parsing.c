
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include "parsing.h"

char *trim_whiteSpaces(char *str){
  // trim leading spaces
  if(str == NULL) return NULL;
  while(isspace((unsigned char)*str) )str++;

  // return if null byte found
    if (*str == '\0'){ return str;}


   char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator
    *(end + 1) = '\0';

    return str;

}


 char* get_command_from_input(char*input){
  while(isspace((unsigned char)*input) )input++; // striping leading whitespaces

      // this section for handling executables with spaces 

      if(input[0]== '\'' || input[0]== '\"'){  // check if command is executable with spaces 
        char quote_char =input[0];
        char* closing_quote =input+1;


        while(*closing_quote && *closing_quote != quote_char){
          if(*closing_quote == '\\' && (*(closing_quote+1) == quote_char  || *(closing_quote+1)=='\\')){
            closing_quote+=2;

          }
          else{
            closing_quote++;
          }
        }



        
        if (*closing_quote != quote_char) {
            return NULL; // No closing quote found
        }

        size_t command_length = closing_quote - (input + 1);
        char* spacecommand = (char*)malloc(command_length + 1);
         int buf_idx = 0;
        for(int i=1;input[i]!=quote_char&& input[i] != '\0';i++){
            if(input[i] == '\\'){
              if(input[i] == '$' ||  input[i] == '\\' || input[i] == '`' || input[i] == '"' || input[i] == '\''){
                spacecommand[buf_idx++] =input[i];
              }
              else{
                spacecommand[buf_idx++]='\\';
                spacecommand[buf_idx++]=input[i];
              }
            }
            else{
              spacecommand[buf_idx++]=input[i];
            }


        

        }
        spacecommand[buf_idx] = '\0'; // Null-terminate
        return spacecommand;

        
          
      }


    char* command_end = input;

    while(*command_end != '\0'&&!isspace((unsigned char)*command_end) )command_end++; // striping leading whitespaces

  size_t command_length = command_end - input;
  char* command = (char*)malloc(command_length + 1);
    if (command == NULL) {
        return NULL; // Memory allocation failed
    }

    strncpy(command, input, command_length);
    command[command_length] = '\0'; // Null-terminate the string

    return command;
 }


char* strip_single_qoutes(char*input){

if(input == NULL)return input ; 

  char *src = input;
  char *dst = input;

    while (*src) {
        if (*src != '\'') {
            *dst++ = *src;
        }
       src++;
    }
    *dst = '\0';

    return input;



}


char* strip_extra_spaces(char*mystring){

if(mystring == NULL) return NULL;

char*src = mystring;
char*dst= mystring;
int space=0;

while (*src){

  if(!isspace((unsigned char)*src)){
    
    if(space>0){
      *dst++=' ';
       space=0;
    }
    *dst++ = *src;
  }
  else{
    space++;
  }
  src++;

  }

  *dst = '\0'; 
  if (mystring[0] == ' ')  // Check if the first character is a space
  {                          
   memmove(mystring, mystring + 1, strlen(mystring)); // Shift the string left by one position
  }


  return mystring;

}




char**            parseArguments(char* input) {
    char** args = malloc(sizeof(char*) * 64); // Allocate space for up to 64 arguments
    int argCount = 0;
    char* currentArg = calloc(256, sizeof(char));  // malloc keeps garabge data and make undefined beahviour , calloc empty the memry before allocating 
    int in_single_quotes= 0 ; 
    int in_double_quotes = 0;
    int in_nested_quotes =0 ;
    
    for (char* p = input; *p != '\0'; p++) {      // traversing the input charachters
        
          if (*p == '"' && !in_single_quotes){
            in_double_quotes = !in_double_quotes;
            continue;                        // here double quotes are starting quotes that won't be included in the arg
          }
        
          else if (*p == '\'' && !in_double_quotes) {       
            in_single_quotes= !in_single_quotes;
              continue;              // same as before , starting or ending single quotes that are not going to be included 
          }   


          else if(*p == '\'' && in_double_quotes){
            in_nested_quotes =!in_nested_quotes;   // single quote inside double quote , will be included as a part of the string 
          }
       




         if (*p == ' ' && !in_single_quotes && !in_double_quotes)   // if a space found outside the quotes  and current arg is not empty 
        {                                                           // means that a word ended , so extra space isn't needed 
            if (strlen(currentArg) > 0) 
            {
                args[argCount++] = strdup(trim_whiteSpaces(currentArg));   //trim any white space in the currentArg and place in it arg
                memset(currentArg, 0, sizeof(char) * 256);                 // Clear the current_arg buffer
            }
        } 
        else   //SPACE inside quotes(included) , any other charachter 
        {

            if( (in_double_quotes && *p == '\\') && !in_nested_quotes){
              
              if(*(p+1) == '$' ||  *(p+1) == '\\' || *(p+1) == '`' || *(p+1) == '"' ||  *(p+1) == '\''){
                p++;
              }
            }
            else if( *p== '\\' && (!in_double_quotes && !in_single_quotes)){
                p++;
            }


            strncat(currentArg, p, 1);       // add this cahrchter to the currentArg buffer 
        }
    }
    
    if (strlen(currentArg) > 0) {                                   //add the last argument in the arg list 
        args[argCount++] = strdup(trim_whiteSpaces(currentArg));
    }

      //print args array for testing   
    // for(int i=0;args[i]!='\0';i++){
    //   printf("%s ",args[i]);
    // }
    
    args[argCount] = NULL; 
    free(currentArg);
    return args;
}
