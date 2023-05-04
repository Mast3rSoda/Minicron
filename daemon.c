#include "h/daemon.h"
#include "h/logging.h"
#include "h/global.h"
#include <string.h>
#include <stdlib.h>
void createDaemon(char* name) //creates daemon
{
    pid_t pid, sid;
    // Fork off parent process
    pid = fork();
    if (pid < 0) //checks if PID viable
    {
        loggingError(name, "Error forking a process.");
        exit(EXIT_FAILURE);
    }

    // If good PID, then we can exit the parent process.
    else if (pid > 0) //if viable then quits parent
    {
        exit(EXIT_SUCCESS);
    }
    umask(0);
    // Create new SID for child process
    sid = setsid(); //sets and checks SID for child
    if(sid < 0)
    {
      loggingError(name, "Error forking a process.");
      exit(EXIT_FAILURE);
    }
}

void execCommand(const char* name, int outputfile, char* command, int mode, int leftCommands) //execute command function
{
  char* ArgumentArray[16];
  char commandCopy[1024], *token, *commandName;
  int i=0;
  char buf[1] = "\n";
  char buf2[1] = " ";
  strcpy(commandCopy, command);
  token = strtok(commandCopy, " "); //takes command name
  commandName = token;
  ArgumentArray[i] = token;
  while( ArgumentArray[i] != NULL ) //writes arguments to array
  {
    i++;
    char* token2;
    token2 = strtok(NULL, " ");
    ArgumentArray[i] = token2;
  }
  loggingmsg(name, "Finished separating arguments!");
  if(!leftCommands) //chooses which output to print to file
  {
    if(mode == 0)
    {
      write(outputfile, buf2, sizeof(buf2)); //writes a space
      dup2(outputfile, STDOUT_FILENO); //standard output
    }
    else if(mode == 1)
    {
      write(outputfile, buf2, sizeof(buf2));
      dup2(outputfile, STDERR_FILENO); //error output
    }
    else if(mode == 2)
    {
      write(outputfile, buf2, sizeof(buf2));
      dup2(outputfile, STDOUT_FILENO); //both outputs
      dup2(outputfile, STDERR_FILENO);
    }
    else
    {
      loggingError(name, "Invalid mode argument.");
    }
    i=0;
    write(outputfile, buf, sizeof(buf)); //writes a new line
  }
  loggingmsg(name, "Command is being executed!");
    execvp(commandName, ArgumentArray); //executes command
  exit(EXIT_FAILURE);
}
