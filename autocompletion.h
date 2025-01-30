#ifndef AUTOCOMPLETION_H
#define AUTOCOMPLETION_H

int autocompelte(char *input, int *idx);
void enable_raw_mode();
void disable_raw_mode();
void init_entries_buffer();
void sort_entries_buffer();
void print_entries_buffer(char *input);
void set_suggestion_buffer(char *input);
void sort_entries_buffer_on_size();
void free_buffer();


#endif