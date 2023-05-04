#ifndef LOG_H
#define LOG_H

#include <syslog.h>

void loggingError(const char*, char*);
void loggingmsg(const char*, char*);

#endif
