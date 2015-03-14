/* Charles Sedgwick
 * CMPT496W15
 *
 * Implementation of oom_restart.h functions
 */

#include <linux/kernel.h>
#include <linux/oom_restart.h>

void oom_restart(void){

	printk( KERN_INFO "oom_restart has been called!" );

	}
