#ifndef __INCLUDE_LINUX_OOM_RESTART_H
#define __INCLUDE_LINUX_OOM_RESTART_H
/* Charles Sedgwick
 * CMPT496W15
 * This file contains the functions that allow the kernel to restart a process
 * that it has killed due to a oom situation. 
 * 
 * A process that has been killed is eligible for restart if it has been running
 * for minimum amount of time. This is to prevent the kernel getting stuck in a
 * loop where it constantly restarts a process that quickly causes an oom
 * situiation. 
 */

#define SIZE_CMD 80
#define SIZE_PAGE 4096
#define SIZE_PID 32768

struct restart_struct{
	pid_t pid;
	char cmdline[SIZE_CMD];
	unsigned long mem_allocd;
	unsigned long mem_growth;
	u64 start_time;
	};

extern unsigned long calc_mem_growth( struct restart_struct *r );
extern void restart_init( struct restart_struct *r, struct task_struct *p );
extern void oom_restart( struct restart_struct *r );

#endif
