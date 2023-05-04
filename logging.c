#include "h/logging.h"
#include "h/global.h"
#include <string.h>
#include <stdlib.h>

void loggingError(const char* name, char* msg) //error logging function
{
    openlog(name, LOG_PID | LOG_CONS, LOG_DAEMON);
    syslog(LOG_ERR, "%s", msg);
    closelog();
}

void loggingmsg(const char* name, char* msg) //message logging function
{
    openlog(name, LOG_PID | LOG_CONS, LOG_DAEMON);
    syslog(LOG_INFO, "%s", msg);
    closelog();
}
