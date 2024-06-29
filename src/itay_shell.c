#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include "itay_shell.h"


int prompt()
{
    int retval = 0;
    char absolute_path[ABOSOLUTE_PATH_MAX_SIZE] = {0};
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


int read_command_line(char *command, size_t length)
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


int is_end_of_string(char c)
{
    return c == '\n' || c == '\0';
}


int append_strings_with_slash(char *const dst, size_t dst_size, const char *const src)
{
    int retval = 0;
    if (dst == NULL || src == NULL)
    {
        retval = -1;
        goto cleanup;
    }
    if (strlen(dst) + strlen(src) + 2 >= dst_size)
    {
        retval = -2;
        goto cleanup;
    }
    strncat(dst, src, strlen(src) + 1);
    strncat(dst, "/", 2);
    retval = (int)(strlen(src)) + 1;

cleanup:
    return retval;
}


int store_and_clear_argument(char* const argument, size_t argument_size, char **const command_args, size_t *argument_number)
{
    int retval = 0;
    char *argument_cp = strdup(argument);
    if (argument_cp == NULL)
    {
        retval = -1;
        goto cleanup;
    }
    command_args[*argument_number] = argument_cp;
    memset(argument, 0, argument_size);
    *argument_number += 1;

cleanup:
    return retval;
}


int write_username_to_argument(
    size_t i,
    size_t username_start_index,
    char *const argument, 
    size_t argument_size, 
    size_t *argument_pos,
    char *const command
    )
{
    int retval = 0;
    char username[USERNAME_MAX_SIZE] = {0};
    memcpy(username, &command[username_start_index + 1], i - username_start_index - 1);
    if (strlen(username) == 0)
    {
        /** Default - use the $HOME env variable */
        int appended_bytes_count = append_strings_with_slash(argument, argument_size, getenv("HOME"));
        if (appended_bytes_count < 0)
        {
            retval = -1;
            goto cleanup;
        }
        *argument_pos += appended_bytes_count;
    }
    else
    {
        /** Found a candidate username */
        const struct passwd *const user_pwd = getpwnam(username);
        if (user_pwd == NULL || user_pwd->pw_dir == NULL)
        {
            /** No such user, leave the piece unmodified */
            if (strlen(argument) + strlen(username) + 3 >= argument_size)
            {
                retval = -2;
                goto cleanup;
            }
            strncat(argument, "~", 2);
            strncat(argument, username, argument_size);
            strncat(argument, "/", 2);
            *argument_pos += strlen(username) + 2;
        }
        else
        {
            /** username found */
            int appended_bytes_count = append_strings_with_slash(argument, argument_size, user_pwd->pw_dir);
            if (appended_bytes_count < 0)
            {
                retval = -3;
                goto cleanup;
            }
            *argument_pos += appended_bytes_count; 
        }
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
    char argument[ARGUMENT_MAX_SIZE] = {0};
    int new_username = 0;
    int prev_was_space = 0;
    size_t username_start_index = 0;
    size_t argument_pos = 0;
    size_t argument_number = 0;
    for (size_t i = 0 ; (i < command_length) && (argument_number < max_command_args_length - 1) ; i++)
    {
        if (!new_username && command[i] == '~')
        {
            prev_was_space = 0;
            new_username = 1;
            username_start_index = i;
        }
        else if (new_username && (command[i] == '/' || is_end_of_string(command[i])))
        {
            prev_was_space = 0;
            new_username = 0;
            if (write_username_to_argument(i, username_start_index, argument, sizeof(argument), &argument_pos, command) < 0)
            {
                retval = -1;
                goto cleanup;
            }
            if (is_end_of_string(command[i]))
            {
                if (store_and_clear_argument(argument, sizeof(argument), command_args, &argument_number) < 0)
                {
                    retval = -1;
                    goto cleanup;
                }
            }
        }

        else if (isspace(command[i]))
        {
            if (prev_was_space) { continue; }
            if (store_and_clear_argument(argument, sizeof(argument), command_args, &argument_number) < 0)
            {
                retval = -1;
                goto cleanup;
            };
            argument_pos = 0;
            prev_was_space = 1;
        }
            
        else if (!new_username)
        {
            if (argument_pos >= sizeof(argument))
            {
                retval = -3;
                goto cleanup;
            }
            argument[argument_pos++] = command[i];
            prev_was_space = 0;
        }
    }

    command_args[argument_number] = NULL;
    *command_args_length = argument_number;

cleanup:
    return retval;
}


int cd_handler(char **command_args, size_t command_args_length)
{
    int retval = 0;
    if (command_args_length != 2)
    {
        printf("cd: takes exactly one argument\n");
        goto cleanup;
    }

    if (chdir(command_args[1]) < 0)
    {
        printf("chdir failed: %s\n", strerror(errno));
        retval = -1;
        goto cleanup;
    }

cleanup:
    return retval;
}

int exec_handler(char **command_args, size_t command_args_length)
{
    int retval = 0;
    if (command_args_length < 2)
    {
        printf("exec: takes at least one argument\n");
        goto cleanup;
    }

    if (execv(command_args[1], command_args + 1) < 0)
    {
        printf("exec failed: %s\n", strerror(errno));
        retval = -1;
        goto cleanup;
    }

cleanup:
    return retval;
}


int try_dispatch_builtin_command(char **command_args, size_t command_args_length)
{
    int retval = 0;
    char *command_program = command_args[0];
    if (strcmp(command_program, "exit") == 0)
    {
        goto cleanup;
    }

    else if (strcmp(command_program, "cd") == 0)
    {
        cd_handler(command_args, command_args_length);
        goto cleanup;
    }

    else if (strcmp(command_program, "exec") == 0)
    {
        exec_handler(command_args, command_args_length);
        goto cleanup;
    }

    else
    {
        retval = -1;
    }

cleanup:
    return retval;
}


int execute_command(char **command_args)
{
    int retval = 0;
    pid_t pid = fork();
    if (pid < 0)
    {
        printf("fork failed: %s\n", strerror(errno));
        retval = -1;
        goto cleanup;
    }

    if (pid != 0)
    {
     /** Parent */   
        int wstatus = 0;
        if (waitpid(pid, &wstatus, 0) < -1)
        {
            retval = -2;
            goto cleanup;
        }

        if (WIFSIGNALED(wstatus))
        {
            printf("terminated by signal: %s\n", strsignal(WTERMSIG(wstatus)));
            retval = -3;
            goto cleanup;
        }
    }
    else
    {
        /** Child */
        if (execv(command_args[0], command_args) < 0)
        {
            printf("child exec failed: %s\n", strerror(errno));
            retval = -4;
        }
        /** Terminate child */
        exit(retval); 
    }

cleanup:
    return retval;
}


int search_file_within_dir(char *filename, char *dirname, char *absolute_path, size_t max_size)
{
    int retval = 0;
    if (dirname == NULL || filename == NULL || absolute_path == NULL)
    {
        retval = -3;
        goto cleanup;
    }
    memset(absolute_path, 0, max_size);

    if (strlen(dirname) + 1 > max_size)
    {
        retval = -4;
        goto cleanup;
    }
    strncpy(absolute_path, dirname, max_size);

    if (strlen(absolute_path) + 1 + strlen(filename) + 1 > max_size)
    {
        retval = -5;
        goto cleanup;
    }

    strcat(absolute_path, "/");
    strcat(absolute_path, filename);

    /** Check if this file does exists */
    struct stat statbuf = {0};
    if (stat(absolute_path, &statbuf) < 0)
    {
        retval = -6;
        goto cleanup;
    }

cleanup:
    return retval;
}


int resolve_executable_path(char **command_args, char **exec_path)
{
    int retval = 0;
    char *const absolute_path = calloc(1, ABOSOLUTE_PATH_MAX_SIZE);
    const size_t absolute_path_size = ABOSOLUTE_PATH_MAX_SIZE;
    if (absolute_path == NULL)
    {
        retval = -1;
        goto cleanup;
    }

    const char *const path_environ = getenv("PATH");
    if (path_environ == NULL)
    {
        retval = -1;
        goto cleanup;
    }

    /** Create a copy of environ, on which we can perform strtok */
    char *const tokenized_path_environ = strdup(path_environ);
    if (tokenized_path_environ == NULL)
    {
        retval = -2;
        goto cleanup;
    }

    char *dirpath = strtok(tokenized_path_environ, ":");
    while (dirpath != NULL)
    {
        if (search_file_within_dir(command_args[0], dirpath, absolute_path, absolute_path_size) != 0)
        {
            retval = -3;
        }
        else
        {
            retval = 0;
            *exec_path = absolute_path;
            break;
        }
        dirpath = strtok(NULL, ":");
    }

cleanup:
    if (retval < -1)
    {
        free(tokenized_path_environ);
    }
    return retval;
}


int try_run_executable_command(char **command_args, size_t command_args_length)
{
    int retval = 0;
    char *exec_path = NULL;

    if (command_args_length < 1)
    {
        retval = -1;
        goto cleanup;
    }

    if (command_args[0][0] != '.' && command_args[0][0] != '/')
    {
        if (resolve_executable_path(command_args, &exec_path) < 0)
        {
            retval = -2;
            goto cleanup;
        }
        /** Swap the first argument */
        free(command_args[0]);
        command_args[0] = exec_path;
    }

    if (execute_command(command_args) < 0)
    {
        retval = -3;
        goto cleanup;
    }
    
cleanup:
    return retval;
}


int parse_command(char *command)
{
    int retval = 0;
    char* command_args[COMMAND_MAX_ARGS] = {0};
    size_t command_args_length = 0;

    size_t command_length = strlen(command);
    if (command_length > COMMAND_MAX_SIZE - 1)
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

    if (try_dispatch_builtin_command(command_args, command_args_length) == 0)
    {
        /** We've successfully run a builtin command, nothing left to do */
        goto cleanup;
    }

    if (try_run_executable_command(command_args, command_args_length) == 0)
    {
        /** We've succesfully run an executable-path command, noting left to do */
        goto cleanup;
    }

    printf("Unrecognized command: %s\n", command_args[0]);
    

cleanup:
    for (size_t i = 0 ; i < command_args_length ; i++)
    {
        free(command_args[i]);
    }
    return retval;
}

int dispatch_commands()
{  
    int retval = 0;
    char command_buffer[COMMAND_MAX_SIZE] = {0}; 
    
    while (1)
    {
        memset(command_buffer, 0, sizeof(command_buffer));

        if (prompt() < 0)
        {
            /** Couldn't print prompt for some obscure reason. Terminate. */
            retval = -1;
            goto cleanup;
        }

        if (read_command_line(command_buffer, sizeof(command_buffer)) < 0)
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
