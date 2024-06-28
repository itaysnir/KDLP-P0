#pragma once

#include <stddef.h>

#define ABOSOLUTE_PATH_MAX_LENGTH 4096
#define COMMAND_MAX_LENGTH 1024
#define COMMAND_MAX_ARGS 128


int prompt();
int read_command(char *command, size_t length);
int parse_command(char *command);
int parse_command_arguments(
    char *command, 
    size_t command_length, 
    char **command_args, 
    size_t max_command_args_length,
    size_t *command_args_length
);
int start_shell();
int dispatch_commands();
