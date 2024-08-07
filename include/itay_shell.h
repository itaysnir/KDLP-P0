#ifndef ITAY_SHELL_H
#define ITAY_SHELL_H

#include <stddef.h>

#define ABOSOLUTE_PATH_MAX_SIZE 4096
#define COMMAND_MAX_SIZE 4096
#define COMMAND_MAX_ARGS 128
#define USERNAME_MAX_SIZE 256
#define ARGUMENT_MAX_SIZE 1024

extern const int PIPE_INVALID;
extern const size_t PIPE_READ;
extern const size_t PIPE_WRITE;


int start_shell();

#endif
