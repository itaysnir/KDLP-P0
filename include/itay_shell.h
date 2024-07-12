#ifndef ITAYSHELL_H
#define ITAYSHELL_H

#include <stddef.h>

#define ABOSOLUTE_PATH_MAX_SIZE 4096
#define COMMAND_MAX_SIZE 4096
#define COMMAND_MAX_ARGS 128
#define USERNAME_MAX_SIZE 256
#define ARGUMENT_MAX_SIZE 1024

typedef enum {
    INTERMEDIATE_COMMAND = 0,
    FIRST_COMMAND = 1,
    LAST_COMMAND = 2,
    FIRST_AND_LAST_COMMAND = 3,
} e_command_order;

int start_shell();

#endif
