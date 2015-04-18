/******************************
 * Charles Sedgwick
 * CMPT496
 * Constains the definitions for the functons dedicated to logging
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <sys/stat.h>
#include "logging.h"
#include "memwatch.h"

const char *log_types[] = { "INFO: ",
				"ACTION: ", 
				"WARNING: ", 
				"ERROR: ",
				"DEBUG: ",
				"VERBOSE:"};

FILE *fd;

#ifdef PTHREAD
pthread_mutex_t l_log;
#endif

void log_init(){

	char *path = log_path();

	log_create();

	fd = fopen ( path, "a");
	assert(fd != NULL);

	free( path );

}

char *log_path(){

	char *log_path = (char*)calloc(S_LPATH, sizeof(char));
	char *log_var = getenv(LOG_VAR);
	
	if(log_var == NULL){
		strncpy( log_path, HOME, S_LPATH );
		strncat( log_path, LOG_PATH, S_LPATH );
	} else {
		strncpy( log_path, log_var, S_LPATH );
		strncat( log_path, "/restarter.log", S_LPATH );
	}

	return log_path;
}

void log_write(int type, char *mess){
	char *path = log_path();
	time_t cur_time = time(NULL);
	char date_time[26];
	char *type_mess = (char*)calloc(S_LOGMESS + sizeof(log_types[type]), sizeof(char));

	if( fd == NULL ){
		printf("ERROR: %s\n", strerror(errno) );
		assert( fd != NULL );
	}

	strftime(date_time, sizeof(date_time), "%c", localtime( &cur_time ) );
	strncat(type_mess, log_types[type], S_LOGMESS + sizeof(log_types[type]) );
	strncat(type_mess, mess, S_LOGMESS + sizeof(log_types[type]) );
#ifdef PTHREAD
	pthread_mutex_lock( &l_log );
#endif
	fprintf( fd, "[ %s ] %s\n", date_time, type_mess);
	fflush(fd);
#ifdef PTHREAD
	pthread_mutex_unlock( &l_log );
#endif
	
	free(path);
	free(type_mess);
}

void log_create(){
	char *path = log_path();
	time_t cur_time = time(NULL);
	char date_time[26];
#ifdef DEBUG
	printf("log_path: %s\n", path);
#endif
	fd = fopen (path, "w");
	assert(fd != NULL);
	
	strftime(date_time, sizeof(date_time), "%c", localtime( &cur_time ) );
#ifdef PTHREAD
	pthread_mutex_lock( &l_log );
#endif
	fprintf(fd, "[ %s ] INFO: Restarter log created.\n",
			date_time);
#ifdef PTHREAD
	pthread_mutex_unlock( &l_log );
#endif

	free(path);
	fclose(fd);
}

void log_close(){
	fclose(fd);
}
