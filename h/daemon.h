#ifndef DAEMON_H
#define DAEMON_H

#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

void createDaemon();
void execCommand(const char*, int, char*, int, int);

#endif
