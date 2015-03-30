#ifndef __INCLUDE_LINUX_OOM_RESTART_H
#define __INCLUDE_LINUX_OOM_RESTART_H
/* Charles Sedgwick
 * CMPT496W15
 * This file contains the functions that allow the kernel to restart a process
 * that it has killed due to a oom situation. 
 * 
 * A process that has been killed is eligible for restart if its rateof memeory
 * use is below a certain threshold. This is to prevent the kernel getting stuck
 * in a loop where it constantly restarts a process that quickly causes an oom
 * situiation. 
 */

#define SIZE_CMD 80
#define SIZE_PAGE 4096
#define SIZE_PID 32768
#define SIZE_ARG 50
#define SIZE_ENV 80
#define MAX_ARGS 20
#define MAX_ENV 20

struct restart_struct{
	pid_t pid;
	char cmdline[SIZE_CMD + 1];
	unsigned long mem_allocd;
	unsigned long mem_growth;
	u64 start_time;
	};

extern unsigned long calc_mem_growth( struct restart_struct *r );
extern void restart_init( struct restart_struct *r, struct task_struct *p );
extern int get_args( char *cmdline, char *argv[] );
extern void oom_restart( struct restart_struct *r );

#endif
