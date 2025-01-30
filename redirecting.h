#ifndef REDIRECTING_H
#define REDIRECTING_H

int handle_redirections(char **args);
static void remove_redirection_tokens(char **args, int start);
void reset_fd();
void save_fd();

#endif // REDIRECTING_H