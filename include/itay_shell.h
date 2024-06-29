#pragma once

#include <stddef.h>

#define ABOSOLUTE_PATH_MAX_LENGTH 4096
#define COMMAND_MAX_LENGTH 1024
#define COMMAND_MAX_ARGS 128


int prompt();
int read_command_line(char *command, size_t length);
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
int cd_handler(char **command_args, size_t command_args_length);
int exec_handler(char **command_args, size_t command_args_length);
int try_dispatch_builtin_command(char **command_args, size_t command_args_length);
int execute_command(char **command_args);
int try_run_executable_command(char **command_args, size_t command_args_length);
