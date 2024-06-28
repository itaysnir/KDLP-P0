#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include "itay_shell.h"


int prompt()
{
    int retval = 0;
    char absolute_path[ABOSOLUTE_PATH_MAX_LENGTH] = {0};
    char* path = getcwd(absolute_path, sizeof(absolute_path));
    if (path == NULL)
    {
        retval = -1;
        goto cleanup;
    }
    if (printf("%s$ ", path) < 0)
    {
        retval = -2;
        goto cleanup;
    };

cleanup:
    return retval;
}


int read_command(char *command, size_t length)
{
    int retval = 0;
    if (fgets(command, (int)length, stdin) == NULL)
    {
        retval = -1;
        goto cleanup;
    }

cleanup:
    return retval;
}

int parse_command_arguments(
    char *command, 
    size_t command_length, 
    char **command_args, 
    size_t max_command_args_length, 
    size_t *command_args_length)
{
    int retval = 0;
    size_t argument_number = 0;
    size_t argument_start_index = 0;
    for (size_t i = 0 ; i < command_length && argument_number < max_command_args_length ; i++)
    {
        if (isspace(command[i]))
        {
            size_t argument_size = i - argument_start_index + 1;
            char *argument = (char *)calloc(1, argument_size);
            if (argument == NULL)
            {
                retval = -1;
                goto cleanup;
            }
            memcpy(argument, &command[argument_start_index], argument_size - 1);
            command_args[argument_number] = argument;
            argument_number += 1;
            argument_start_index += argument_size; 
        }
    }

    *command_args_length = argument_number;

cleanup:
    return retval;
}


int parse_command(char *command)
{
    int retval = 0;
    char* command_args[COMMAND_MAX_ARGS] = {0};
    size_t command_args_length = 0;

    size_t command_length = strlen(command);
    if (command_length > COMMAND_MAX_LENGTH - 1)
    {
        retval = -1;
        goto cleanup;
    }

    if (command_length == sizeof(char))
    {
        /** Only '\n' was given as a command, do not process */
        goto cleanup;
    }

    if (parse_command_arguments(command, command_length, command_args, sizeof(command_args), &command_args_length) < 0)
    {
        retval = -2;
        goto cleanup;
    }

    char *command_program = command_args[0];
    if (strcmp(command_program, "exit") == 0)
    {
        goto cleanup;
    }

    else if (strcmp(command_program, "cd") == 0)
    {
        goto cleanup;
    }


    else
    {
        printf("Unrecognized command: %s\n", command_program);
    }


cleanup:
    return retval;
}

int dispatch_commands()
{  
    int retval = 0;
    char command_buffer[COMMAND_MAX_LENGTH] = {0}; 
    
    while (1)
    {
        memset(command_buffer, 0, sizeof(command_buffer));

        if (prompt() < 0)
        {
            /** Couldn't print prompt for some obscure reason. Terminate. */
            retval = -1;
            goto cleanup;
        }

        if (read_command(command_buffer, sizeof(command_buffer)) < 0)
        {
            /** EOF sent */
            retval = 0;
            goto cleanup;
        }

        if (parse_command(command_buffer) < 0)
        {
            /** Parsing error */
            retval = -2;
            goto cleanup;
        }
    }

cleanup:
    return retval;
}


int start_shell()
{
    int retval = 0;
    retval = dispatch_commands();

    return retval;
}
