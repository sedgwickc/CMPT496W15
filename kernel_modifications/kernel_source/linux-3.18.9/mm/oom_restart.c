/* Charles Sedgwick
 * CMPT496W15
 *
 * Implementation of oom_restart.h functions
 */

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/oom_restart.h>
#include <linux/stat.h>
#include <linux/fcntl.h>
#include <linux/mm.h>   /* needed for get_cmline() */

void oom_restart(struct task_struct *p){
	
	pid_t pid = p->pid;
	char cmdline[SIZE_CMD];
	
	printk( KERN_INFO "-----OOM_RESTART-----" );
	
	if( get_cmdline(p, cmdline, SIZE_CMD ) <= 0 )
	{
		printk( KERN_INFO "ERROR_OOM_RESTART: cmdline not copied" );
		return;
	}	

	printk( KERN_INFO "Task %u killed", pid);
	printk( KERN_INFO "get_cmdline() result: %s", cmdline );
	
	}
