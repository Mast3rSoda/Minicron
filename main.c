#include "h/task.h"
#include "h/logging.h"
#include "h/global.h"
#include "h/daemon.h"
#include "h/signals.h"
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

tasksList *firstNode;
tasksList *temp;
const char* signalInputFile; //to be used in signals
const char* signalOutputFile;

int main(int argc, char const *argv[])
{
  signal(SIGINT, signalSIGINT); //signal handler
  signal(SIGUSR1, signalSIGUSR1);
  signal(SIGUSR2, signalSIGUSR2);

  if(argc != 3) // check if right amount of arguments
  {
      loggingError(argv[0], "Not enough arguments!");
      exit(EXIT_FAILURE);
  }
  createDaemon(); //create daemon function
  int inputfile = open(argv[1], O_RDONLY); // file open
  int outputfile = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU | S_IRWXG | S_IWOTH);
  signalInputFile = argv[1];
  signalOutputFile = argv[2];
  if(inputfile == -1) // check if input file opened successfuly
  {
    loggingError(argv[0], "Error opening input file");
    exit(EXIT_FAILURE);
  }
  if(outputfile == -1) // check if output file opened successfuly
  {
    loggingError(argv[0], "Error opening output file");
    exit(EXIT_FAILURE);
  }
  char buffer[2048];
  read(inputfile, buffer, sizeof(buffer)); //reads from file
  char *line = strtok(buffer, "\n");
  char* arr[100];
  arr[0] = line;
  int i=0;
  while(arr[i] != NULL) //separetes lines from each other
  {
    i++;
    char* comp;
    comp = strtok(NULL,"\n");
    if(comp != NULL && strchr(comp, ':') != NULL)
      arr[i] = comp;
    else
    {
      loggingmsg(argv[0], "Finished separating lines from file!");
      break;
    }
  }
  int y=0;
  while(y < i) //separetes variables in lines
  {
    char* line = strtok(arr[y], ":");
    int hour = atoi(line);
    int minute = atoi(strtok(NULL, ":"));
    char *command = strtok(NULL, ":");
    int mode = atoi(strtok(NULL,":"));
    temp = addToList(temp, hour, minute, command, mode); //adds to linked list
    y++;
  }
  loggingmsg(argv[0], "Finished separating variables from lines!");
  firstNode = temp;
  while(1)
  {
    int sleepDuration = getSleepDuration(temp); //gets sleep duration in seconds
    while(sleepDuration<0) //checks if tasks for the day aren't over
    {
      if(temp->next != NULL)
        temp = temp->next;
      sleepDuration = getSleepDuration(temp);
      if(sleepDuration<0 && temp->next == NULL)
      {
        temp = firstNode;
        sleepDuration = getSleepDuration(temp);
        sleepDuration += 86400; //adds a days worth of seconds to earliest task
        break;
      }
    }
    sleep(sleepDuration); //daemon goes to sleep
    printCommand(temp, outputfile); //prints command
    char* commandArray[16]; // max 16 commands to pipe
    int howManyPipes;
    howManyPipes = checkForPipes(temp, commandArray); //check how many pipes
    if(!howManyPipes) //if no pipes found
    {
      pid_t childPID = fork(); //creates child
      if (childPID < 0)
      {
          loggingError(argv[0], "Error forking a process.");
          exit(EXIT_FAILURE);
      }
      if(childPID == 0)
      {
        execCommand(argv[0], outputfile, temp->command, temp->mode, howManyPipes); //executes command
        loggingError(argv[0], "Error executing command");
        exit(EXIT_FAILURE);
      }
    }
    else
    {
      int mypipe[2], in, x;
      x = howManyPipes;
      in = STDIN_FILENO;
      while(howManyPipes+1)
      {
        if(pipe(mypipe) == -1) //checks if pipe created successfuly
        {
          loggingError(argv[0], "Error creating pipe");
          exit(EXIT_FAILURE);
        }
        pid_t childPID = fork(); //creates child
        if (childPID < 0)
        {
          loggingError(argv[0], "Error forking a process.");
          exit(EXIT_FAILURE);
        }
        else if(childPID == 0)
        {
          if(in != 0)
          {
            dup2(in, STDIN_FILENO); //takes input from standard input
            close(in);
          }
          dup2(mypipe[1], STDOUT_FILENO); //puts output in standard output
          close(mypipe[1]);
          if(!howManyPipes)
          {
            in = mypipe[0];
            dup2(mypipe[1], 1);
          }
          execCommand(argv[0], outputfile, commandArray[x-howManyPipes], temp->mode, howManyPipes); //executes command
          exit(EXIT_FAILURE);
        }
        else
        {
          int status;
          wait(&status); //waits for child to finish
        }
        close(in);
        close(mypipe[1]); //closes pipes
        in = mypipe[0];
        howManyPipes--;
      }
    }
    sleep(1);
    if(temp->next != NULL) //goes to next task
      temp = temp->next;
  }
  exit(EXIT_SUCCESS);
}
