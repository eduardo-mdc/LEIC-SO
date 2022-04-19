#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    char buf[1024];
    char *command;
    char *commands[100];
    char delim[] = " ";
    char *buffer;
    pid_t pid;
    /* do this until you get a ^C or a ^D */
    for (;;)
    {
        /* give prompt, read command and null terminate it */
        fprintf(stdout, "$ ");
        if ((command = fgets(buf, sizeof(buf), stdin)) == NULL)
            break;
        command[strlen(buf) - 1] = '\0';
        /* call fork and check return value */
        if ((pid = fork()) == -1)
        {
            fprintf(stderr, "%s: can't fork command: %s\n",
                    argv[0], strerror(errno));
            continue;
        }
        else if (pid == 0)
        {   
            /* child */

            //split commands
            char *token = strtok(command,delim);
            int counter = 0;
            while( token != NULL ) {
                commands[counter] = strdup(token);
                token = strtok(NULL,delim);
                counter++;
            }
            commands[counter] = NULL;

            if(strcmp(commands[0],"myhistory") == 0){
                commands[0] = "history";
                commands[1] = "|";
                commands[2] = "tail";
                commands[3] = "-n";
                commands[4] = "10";
                commands[5] = NULL;
            }

            execvp(commands[0], commands);
            /* if I get here "execvp" failed */
            fprintf(stderr, "%s: couldn't exec %s: %s\n",
                    argv[0], buf, strerror(errno));
            /* terminate with error to be caught by parent */
            exit(EXIT_FAILURE);
        }
        /* shell waits for command to finish before giving prompt again */
        if ((pid = waitpid(pid, NULL, 0)) < 0)
            fprintf(stderr, "%s: waitpid error: %s\n",
                    argv[0], strerror(errno));
    }
    exit(EXIT_SUCCESS);
}