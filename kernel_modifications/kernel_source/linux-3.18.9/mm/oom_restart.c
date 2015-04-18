/* Charles Sedgwick
 * CMPT496W15
 *
 * Implementation of oom_restart.h functions
 */

#include <linux/sched.h>
#include <linux/stat.h>
#include <linux/fcntl.h>
#include <linux/mm.h>   /* needed for get_cmline() */
#include <linux/string.h>
#include <linux/slab.h>
#include <uapi/linux/binfmts.h>
#include <linux/oom_restart.h>

int (*restart_cmd_send)(char *) = NULL;

void set_cmd_func( int (*cmd_func)(char*) ){
	restart_cmd_send = cmd_func;
}
EXPORT_SYMBOL(set_cmd_func);

unsigned long calc_mem_growth( struct restart_struct *r )
{
	struct timespec now, boot;
	getnstimeofday( &now );
	getboottime( &boot );
	return r->mem_allocd / (now.tv_sec - (boot.tv_sec + r->start_time)); /* KB/second */
}

void restart_init( struct restart_struct *r, struct task_struct *p )
{
	int i;
	
	r->pid = p->pid;
	r->start_time = p->real_start_time / 1000000000; /*start time in seconds */
	r->mem_allocd = p->mm->hiwater_rss * 4096 / 1024; /* define macros for this! */
	r->mem_growth = calc_mem_growth( r );

	memset( r->cmdline, 0, SIZE_CMD + 1 );
	if( get_cmdline(p, r->cmdline, SIZE_CMD-1 ) <= 0 )
	{
		pr_err( "[ERROR] oom_restart(): cmdline not copied" );
		return;
	}	

	for( i = 0; i < SIZE_CMD; i++ )
	{
		if( r->cmdline[i] == '\0' )
		{
			r->cmdline[i] = ';';
		}
	}
}

/* check out argv_split in /include/linux/string.h */
int parse_cmdline( char *cmdline, char *argv[] )
{
	char *cmd;
	char *tok;
	char del;
	int i;

	if( cmdline == NULL || argv == NULL )
	{
		pr_err( "NULL value passed to get_args(char*, char**)" );
		return -1;
	}
	cmd = cmdline;
	del = ';';

	tok = strsep( &cmd, &del );
	for( i = 0; i < MAX_ARGS; i++ )
	{
		if( tok != NULL )
		{
			argv[i] = kzalloc( MAX_STRLEN + 1, GFP_KERNEL );
			if( argv[i] == NULL )
			{
				pr_err( "Kernel unable to allocate memory for argument" );
				for(; i >= 0; i-- )
				{
					kfree(argv[i]);
				}
				return -1;
			}
			
			if( i == 0 )
			{
				strncpy(argv[i], "/usr/bin/", 9);
				strncat( argv[i], tok, MAX_STRLEN - 9 );
			}
			else
			{
				strncpy( argv[i], tok, MAX_STRLEN );
			}
			tok = strsep( &cmd, &del );
		}
		else 
		{
			argv[i] = NULL;
		}
	}

	return 0;
}

int restart_module_loaded(){
	if( restart_cmd_send != NULL )
		return 1;
	else
		return 0;
}

/* oom_restart()
 * params
 * struct restart_struct *r
 * TODO:
 * - restart: only restart killed task if its mem_growth isn't "high"
 * - get environment variables for task
 */
void oom_restart(struct restart_struct *r)
{
	int rc;

	if( r == NULL )
	{
		pr_err( "OOM_RESTART: NULL passed instead of restart_struct*");
		return;
	}

	pr_err( "-----OOM_RESTART-----" );
	pr_err( "Task %u killed", r->pid);
	pr_err( "Memory use rate: %lu KB/second", r->mem_growth);

	if( r->mem_growth < GROWTH_THRESH )
	{
		pr_err( "OOM_RESTART: sending message to userland" );
		if( restart_module_loaded() )
			rc = restart_cmd_send( r->cmdline );
		else
		{
			pr_err( "Restart module not loaded" );
			return;
		}
		if( rc < 0 )
			pr_err( "OOM_RESTART: restart_cmd_send returned with error: %d", rc );
	}

	return;
	/* 
		pr_err( "Attempting restart of %s", argv[0] );
		if( (ret = call_usermodehelper( argv[0], argv, dummy_env, UMH_WAIT_PROC
			)) == 0 )
		{
			pr_err( "%s successfully restarted!", argv[0] );
		}
		else
		{
			pr_err( "Unable to restart %s. Error # %d", argv[0], ret );
		}
	}
*/
}
