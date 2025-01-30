#ifndef PARSING_H
#define PARSING_H

char *trim_whiteSpaces(char *str);
char* get_command_from_input(char*input);
char* strip_single_qoutes(char*input);
char* strip_extra_spaces(char*mystring);
char** parseArguments(char* input);











#endif
