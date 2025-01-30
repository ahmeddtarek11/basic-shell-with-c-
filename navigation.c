#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "navigation.h"
#include "parsing.h"



void pwd(){
   char buffer[1024];
   getcwd(buffer,1024);
   if(buffer == NULL){
    perror("buffer");
   }
    else{
        printf("%s\n",buffer);
    }
  

}

void cd (char* input){

    char* command = strtok(input, " ");
    char* path = strtok(NULL, " ");
    if(path == NULL){
    perror ("path");
    return;
    }
    else if ( strcmp(path,"~") == 0 ) {
    const char *home = getenv("HOME");
        chdir(home);
        return;
    }
    
    int stat = chdir(path);
    if (stat== -1) perror (path);
    


}


