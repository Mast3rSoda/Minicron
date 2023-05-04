#ifndef TASK_H
#define TASK_H

#include <time.h>
#include <stdlib.h>

typedef struct taskList
{
    int hour;
    int minutes;
    char command[1024];
    int mode;
    struct taskList * next;
} tasksList;

tasksList* createNode(int, int, char*, int);
tasksList* addToList(tasksList*, int, int, char*, int);
int getSleepDuration(tasksList*);
int checkForPipes(tasksList* , char**);
void printCommand(tasksList*, int);
#endif
