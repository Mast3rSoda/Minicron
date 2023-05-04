#include "h/task.h"
#include "h/global.h"
#include <string.h>
#include <stdlib.h>


tasksList* createNode(int hour, int minutes, char* command, int mode) //creates a node in linked list
{
  tasksList* tamp = malloc(sizeof(tasksList));
  tamp->hour = hour;
  tamp->minutes = minutes;
  strcpy(tamp->command, command);
  tamp->mode = mode;
  return tamp;
}
//using linked list since unknown size of file
tasksList* addToList(tasksList* pointer, int hour, int minutes, char* command, int mode)
{
  if(pointer == NULL) //if first command
  {
    pointer = createNode(hour, minutes, command, mode);
    pointer->next = NULL;
    return pointer;
  }
  else //every other command (getting sorted at writing to list)
  {
    tasksList *tamp, *tmp;
    tamp = pointer;
    tmp = pointer;
    int startMin, startMinNext;
    startMin = hour*60 + minutes; //gets starttime in minutes
    startMinNext = tamp->hour*60 + tamp->minutes;
    if(startMin < startMinNext) //checks if first in sorted linked list
    {
      tasksList* tampI;
      tampI = createNode(hour, minutes, command, mode);
      tampI->next = pointer;
      pointer = tampI;
      return(pointer);
    }
    else if(tamp->next != NULL) //checks if in the middle
    {
      while(tamp->next != NULL)
      {
        tamp = tamp->next;
        startMinNext = tamp->hour*60 + tamp->minutes;
        if(startMin < startMinNext)
        {
          tasksList* tampI;
          tampI = createNode(hour, minutes, command, mode);
          tampI->next = tamp;
          tmp->next = tampI;
          return(pointer);
        }
        tmp = tmp->next;
      }
      if(tamp->next == NULL) //if last in linked list
      {
      tasksList* tampI;
      tampI = createNode(hour, minutes, command, mode);
      tamp->next = tampI;
      tampI->next = NULL;
      return(pointer);
      }
    }
    else //just in case puts it as last
    {
      tasksList* tampI;
      tampI = createNode(hour, minutes, command, mode);
      tamp->next = tampI;
      tampI->next = NULL;
      return(pointer);
    }
  }
}

int getSleepDuration(tasksList* pointer) //gets sleep duration in seconds
{
  time_t rawtime;
  time(&rawtime); //gets time in utc
  struct tm* local = localtime(&rawtime); //change to utc+2
  int localSec = local->tm_hour*3600 + local->tm_min*60 + local->tm_sec; //extract time in seconds to int
  int waitTime = pointer->hour*3600 + pointer->minutes*60 - localSec; // get time to next task
  return(waitTime);
}

int checkForPipes(tasksList* pointer, char** command) //checks for amount of commands and pipes to do
{
  char* commandCpy = (char *) malloc(strlen(pointer->command)+1);
  char *token;
  int i=0, howManyPipes = 0;
  strcpy(commandCpy, pointer->command);
  token = strtok(commandCpy, "|");
  command[0] = token;
  while( command[i] != NULL )
  {
    i++;
    char* token2;
    token2 = strtok(NULL, "|");
    command[i] = token2;
    howManyPipes++;
  }
  howManyPipes--;
  return(howManyPipes); //return amount of piping to do (amount of commands - 1)
}

void printCommand(tasksList* pointer, int outputfile) //prints commands name
{
  char buf[1] = "\n";
  write(outputfile, buf, sizeof(buf));
  write(outputfile, pointer->command, strlen(pointer->command));
  char buf2[1] = ":";
  write(outputfile, buf2, sizeof(buf2));
}
