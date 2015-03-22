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

#define SIZE_PID 32768
#define SIZE_CMD 80

extern void oom_restart(struct task_struct *p);

#endif
