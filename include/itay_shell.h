#pragma once

#include <stddef.h>

#define ABOSOLUTE_PATH_MAX_SIZE 4096
#define COMMAND_MAX_SIZE 1024
#define COMMAND_MAX_ARGS 128
#define USERNAME_MAX_SIZE 256
#define ARGUMENT_MAX_SIZE 4096


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
int search_file_within_dir(char *filename, char *dirname, char *absolute_path, size_t max_size);
int resolve_executable_path(char **command_args, char **exec_path);
int is_end_of_string(char c);