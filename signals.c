#include <signal.h>
#include "h/signals.h"
#include "h/global.h"
#include "h/logging.h"
#include "h/task.h"

void signalSIGINT(int sig) //checks for SIGINT signal
{
    if(sig == SIGINT)
    {
      loggingmsg("./minicron.o", "Quitting main daemon!");
      exit(EXIT_SUCCESS); //quits if signal has been found
    }
}

void signalSIGUSR1(int sig) //checks for SIGUSR1 signal
{
  if(sig == SIGUSR1)
  {
    loggingmsg("./minicron.o", "Re-reading data from file!");
    execl("./minicron.o", "./minicron.o", signalInputFile, signalOutputFile,(char*) NULL); //restarts the programm
  }

}

void signalSIGUSR2(int sig) //checks for SIGUSR2 signal
{
  loggingmsg("./minicron.o", "Printing commands yet to be executed!");
  tasksList* tmp = firstNode;
  while(tmp != temp)
  {
    tmp = tmp->next;
  }
  loggingmsg("./minicron.o", tmp->command);
  while(tmp->next != NULL)
  {
    tmp = tmp->next;
    loggingmsg("./minicron.o", tmp->command); //logs all messages that have been executed
  }

  int sleepDuration = getSleepDuration(temp);
  while(sleepDuration<0) //finds the sleep duration to next task
  {
    if(temp->next != NULL)
      temp = temp->next;
    sleepDuration = getSleepDuration(temp);
    if(sleepDuration<0 && temp->next == NULL)
    {
      temp = firstNode;
      sleepDuration = getSleepDuration(temp);
      sleepDuration += 86400;
      break;
    }
  }
  sleep(sleepDuration); //puts the daemon to sleep until next task
}
