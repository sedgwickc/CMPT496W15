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

unsigned long calc_mem_growth( struct restart_struct *r )
{
	struct timespec now, boot;
	getnstimeofday( &now );
	getboottime( &boot );
	return r->mem_allocd / (now.tv_sec - (boot.tv_sec + r->start_time)); /* MB/second */
}

void restart_init( struct restart_struct *r, struct task_struct *p )
{
	int i;
	
	r->pid = p->pid;
	r->start_time = p->real_start_time / 1000000000; /*start time in seconds */
	r->mem_allocd = p->mm->hiwater_rss * 4096 / (1024 *1024);
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
	
	struct timespec tv, boot;
	getnstimeofday( &tv );
	getboottime( &boot );
	printk( KERN_INFO "-----OOM_RESTART-----" );
	printk( KERN_INFO "Task %u killed", r->pid);
	printk( KERN_INFO "get_cmdline() result: %s", r->cmdline );
	printk( KERN_INFO "start time: %lu ",  r->start_time );
	printk( KERN_INFO "Boot time: %lu", boot.tv_sec );
	printk( KERN_INFO "Current time in seconds: %lu", tv.tv_sec );
	printk( KERN_INFO "Memory used: %luMB", r->mem_allocd );
	printk( KERN_INFO "Memory use rate: %lu MB/second", r->mem_growth);
}
