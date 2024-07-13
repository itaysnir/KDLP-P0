#ifndef ITAYSHELL_H
#define ITAYSHELL_H

#include <stddef.h>

#define ABOSOLUTE_PATH_MAX_SIZE 4096
#define COMMAND_MAX_SIZE 4096
#define COMMAND_MAX_ARGS 128
#define USERNAME_MAX_SIZE 256
#define ARGUMENT_MAX_SIZE 1024

static const int PIPE_INVALID = -1;
static const size_t PIPE_READ = 0;
static const size_t PIPE_WRITE = 1;

int start_shell();

#endif
