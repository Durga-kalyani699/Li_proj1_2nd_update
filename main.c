
#include"main.h"

int status=0;
pid_t shell_pid;
char prompt[50]="minishell$";

int main()
{
    shell_pid = getpid();
    system("clear");
    while(1)
    {
        // system("clear");

        printf("%s :",prompt);
        fflush(stdout);

        char input[1024];

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';

        /* Ignore empty input */
        if (strlen(input) == 0)
            continue;

        if(strncmp(input,"PS1=",4)==0)
        {
            if(validate_prompt(input)==0)
            {
                printf("ERROR : Invalid prompt\n");
            }
            else
            {
                printf("INFO : Validation of prompt completed\n");
                strcpy(prompt,input+4);
                printf("INFO : Prompt changed to %s\n",prompt);
            }
            continue;
        }
        

            char *commands[153];

            int i = 0;

            char *saveptr;
            char *token = strtok_r(input, " \t",&saveptr);

            while(token != NULL && i < 152)
            {
                commands[i] = token;
                i++;
                token = strtok_r(NULL, " \t",&saveptr);
            }

            commands[i] = NULL;
            if (commands[0] == NULL)
            continue;
        
        if (check_pipe(commands))
        {
            execute_n_pipe(commands);
        }

        /* Builtin command */
        else if (check_cmd_type(commands[0]) == Builtin_cmd)
        {
            execute_builtin_cmd(commands);
        }
        else if(check_cmd_type(commands[0])==External_cmd)
        {
            printf("INFO : It is a external command\n");
            pid_t ret=fork();
            if (ret < 0)
            {
                perror("fork");
                status = 1;
            }
            else if(ret>0)
            {
                int wstatus;

                printf("INFO : Parent process waiting for child to complete\n");

                if (waitpid(ret, &wstatus, 0) == -1)
                {
                    perror("waitpid");
                    status = 1;
                }
                else if (WIFEXITED(wstatus))
                {
                    status = WEXITSTATUS(wstatus);
                }
                else if (WIFSIGNALED(wstatus))
                {
                    status = 128 + WTERMSIG(wstatus);
                }
            }
            else
            {
                execute_external_cmd(commands);
            } 
        }
        else
        {
            printf("ERROR : Invalid command\n");
            status=127;
        }
    }
    
    return 0;
    
}
int validate_prompt(char str[])
{
    for(int i=0;str[i];i++)
    {
        if(str[i]==' '||str[i]=='\t'||str[i]=='\n')
        {
            return 0;
        }
    }
    if(strlen(str)<=4)
    {
        return 0;
    }
    if (strncmp(str, "PS1=", 4) != 0)
        return 0;
    return 1;
}

int check_cmd_type(char* cmd)
{
    for(int i=0;builtins[i]!=NULL;i++)
    {
        if(strcmp(cmd,builtins[i])==0)
        {
            return Builtin_cmd;
        }
    }
    for(int i=0;externals[i] != NULL;i++)
    {
        if(strcmp(cmd,externals[i])==0)
        {
            return External_cmd;
        }
    }
    return No_cmd;
}

void execute_builtin_cmd(char *commands[])
{
    if(strcmp(commands[0], "cd") == 0)
    {
        char *path = commands[1];

        if(path == NULL)
        {
            path = getenv("HOME");
        }

        if(path == NULL || chdir(path) == -1)
        {
            perror("cd");
        }
    }
    else if(strcmp(commands[0], "pwd") == 0)
    {
        char cwd[1024];

        if(getcwd(cwd, sizeof(cwd)) != NULL)
        {
            printf("%s\n", cwd);
        }
        else
        {
            perror("pwd");
        }
    }
    else if(strcmp(commands[0], "exit") == 0)
    {
        exit(0);
    }
    else if(strcmp(commands[0], "echo") == 0)
    {
        if(strcmp(commands[1],"$SHELL")==0)
        {
            printf("%s\n", getenv("SHELL"));
        }
        else if(strcmp(commands[1],"$$")==0)
        {
            printf("%d\n", (int)shell_pid);
        }
        else if(strcmp(commands[1],"$?")==0)
        {
            printf("%d\n", status);
        }
        else
        {
            for(int i = 1; commands[i] != NULL; i++)
            {
                printf("%s ", commands[i]);
            }
            printf("\n");
        }
        //printf("\n");
    }
}

void execute_external_cmd(char* commands[])
{
        execvp(commands[0], commands);

        perror("execvp");
        exit(127);
    
}

int check_pipe(char *str[])
{
    for (int i = 0; str[i] != NULL; i++)
    {
        if (strcmp(str[i], "|") == 0)
        return 1;
    }

    return 0;
}
void print_n_pipe_usage()
{
    printf("USAGE : command1 | command2 | command3 | ... | commandN\n");
}
void execute_n_pipe(char *commands[])
{
    int pipefd[2];
    int prev_fd = -1;

    pid_t pids[100];
    int count = 0;

    char *args[100];

    int i = 0;

    while (commands[i] != NULL)
    {
        int j = 0;

        /* Collect arguments for current command */
        while (commands[i] != NULL &&
               strcmp(commands[i], "|") != 0)
        {
            args[j++] = commands[i++];
        }

        args[j] = NULL;

        /* Invalid empty command */
        if (j == 0)
        {
            print_n_pipe_usage();
            status = 1;

            if (prev_fd != -1)
                close(prev_fd);

            return;
        }

        int has_next = (commands[i] != NULL);

        if (has_next)
        {
            i++;

            if (commands[i] == NULL ||
                strcmp(commands[i], "|") == 0)
            {
                print_n_pipe_usage();
                status = 1;

                if (prev_fd != -1)
                    close(prev_fd);

                return;
            }

            if (pipe(pipefd) == -1)
            {
                perror("pipe");
                status = 1;

                if (prev_fd != -1)
                    close(prev_fd);

                return;
            }
        }

        pid_t pid = fork();

        if (pid < 0)
        {
            perror("fork");
            status = 1;

            if (prev_fd != -1)
                close(prev_fd);

            return;
        }

        if (pid == 0)
        {
            /* Connect previous pipe to stdin */
            if (prev_fd != -1)
            {
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }

            /* Connect current pipe to stdout */
            if (has_next)
            {
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
            }

            /* Execute builtin or external command */
            if (check_cmd_type(args[0]) == Builtin_cmd)
            {
                execute_builtin_cmd(args);
                _exit(status);
            }
            else
            {
                execute_external_cmd(args);
            }
        }

        /* Parent process */
        pids[count++] = pid;

        if (prev_fd != -1)
            close(prev_fd);

        if (has_next)
        {
            close(pipefd[1]);
            prev_fd = pipefd[0];
        }
        else
        {
            prev_fd = -1;
        }
    }

    /* Wait for all child processes */
    int wstatus;

    for (int k = 0; k < count; k++)
    {
        if (waitpid(pids[k], &wstatus, 0) == -1)
        {
            perror("waitpid");
            status = 1;
            continue;
        }

        if (pids[k] == pids[count - 1])
        {
            if (WIFEXITED(wstatus))
                status = WEXITSTATUS(wstatus);
            else if (WIFSIGNALED(wstatus))
                status = 128 + WTERMSIG(wstatus);
        }
    }
}
