#ifndef GLOBAL_H
#define GLOBAL_H

#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include "task.h"

tasksList *firstNode;
tasksList *temp;
const char* signalInputFile;
const char* signalOutputFile;

#endif
