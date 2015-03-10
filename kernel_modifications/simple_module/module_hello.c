/* Charles Sedgwick
 * Simple hello world kernel module
 */

#include <linux/module.h> /* needed by all modules */
#include <linux/kernel.h> /* needed for KERN_INFO */
#include <linux/init.h> /* needed for macros */
#include <linux/moduleparam.h> /* needed for macros */
#define DRIVER_AUTHOR "Charles Sedgwick <sedgwickcharles@gmail.com>"
#define DRIVER_DESC "A test module"

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("test_device");

/* cmd line args */
int cmd_line_arg = 10;
module_param(cmd_line_arg, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC( cmd_line_arg, "An integer" ); 

/* __initdata */

/* init function */
int __init hello_init(void)
{
	printk( KERN_INFO "Hello world\n" );
	printk( KERN_INFO "Command line argument: %d\n", cmd_line_arg );
	
	/* a non 0 return means the module cannot be loaded */
	return 0;
}

void __exit hello_cleanup(void)
{
	printk( KERN_INFO "Goodbye world. \n" );
}

module_init(hello_init);
module_exit(hello_cleanup);
