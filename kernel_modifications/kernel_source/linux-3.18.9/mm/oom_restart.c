/* Charles Sedgwick
 * CMPT496W15
 *
 * Implementation of oom_restart.h functions
 */

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/stat.h>
#include <linux/fcntl.h>
#include <linux/mm.h>   /* needed for get_cmline() */
#include <linux/oom_restart.h>

int calc_mem_growth( struct restart_struct *r )
{
	return 5; /* dummy value for now */
}

void restart_init( struct restart_struct *r, struct task_struct *p )
{
	int i;
	
	r->pid = p->pid;
	r->user = p->utime;
	r->sys = p->stime;
	r->mem_allocd = 1500; /* dummy value for now */
	r->mem_growth = calc_mem_growth( r );

	memset( r->cmdline, 0, SIZE_CMD );
	if( get_cmdline(p, r->cmdline, SIZE_CMD-1 ) <= 0 )
	{
		printk( KERN_INFO "[ERROR] oom_restart(): cmdline not copied" );
		return;
	}	

	for( i = 0; i < SIZE_CMD; i++ )
	{
		if( r->cmdline[i] == '\0' )
		{
			r->cmdline[i] = ' ';
		}
	}
}

/* oom_restart()
 * params
 * struct restart_struct *r
 * TODO:
 * - restart: only restart killed task if its mem_growth isn't "high"
 */
void oom_restart(struct restart_struct *r){
	
	printk( KERN_INFO "-----OOM_RESTART-----" );
	printk( KERN_INFO "Task %u killed", r->pid);
	printk( KERN_INFO "get_cmdline() result: %s", r->cmdline );
	printk( KERN_INFO "Time alive-> utime: %lld, stime: %lld", r->user, r->sys );
	
}
