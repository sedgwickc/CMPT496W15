#ifndef LOGGING_H
#define LOGGING_H

#ifdef PTHREAD
#include <pthread.h>
#endif

#define S_ERRMESS 128
#define S_LOGMESS 256
#define S_LPATH 128

#define HOME getenv("HOME")
#define LOG_PATH "/restarter.log"
#define LOG_VAR "RESTARTERLOG"
#define LOG_INFO 0
#define LOG_ACT 1
#define LOG_WARN 2
#define LOG_ERR 3
#define LOG_DEBUG 4
#define LOG_VERB 5

const char *log_types[6];

#ifdef PTHREAD
extern pthread_mutex_t l_log;
#endif

void log_init();

char *log_path();

void log_write(int type, char *mess);

void log_create();

void log_close();

#endif
