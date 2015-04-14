/* Charles Sedgwick
 */
#include <linux/module.h> /* needed by all modules */
#include <linux/kernel.h> /* needed for KERN_INFO */
#include <linux/init.h> /* needed for macros */
#include <linux/moduleparam.h> /* needed for macros */
#include <linux/oom_restart.h>
#include <net/genetlink.h> /* needed for netlink */
#include <net/netlink.h>

#define DRIVER_AUTHOR "Charles Sedgwick <sedgwickcharles@gmail.com>"
#define DRIVER_DESC "A test module"

#define GROWTH_THRESH 5000
#define RESTART_A_MAX (__RESTART_A_MAX - 1)
#define RESTART_C_MAX (__RESTART_C_MAX - 1)
#define RESTART_GRP "RESTART_GRP" /* user defined group for multicast */

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("test_device");

enum {
	RESTART_A_UNSPEC,
	RESTART_A_MSG,
	__RESTART_A_MAX,
};

enum {
	RESTART_C_UNSPEC,
	RESTART_C_CMD_SEND,
	__RESTART_C_MAX,
};

/* Family Definition */
struct genl_family restart_task_genl_family = {
	.id = GENL_ID_GENERATE,
	.hdrsize = 0,
	.name = "RESTART",
	.version = 1,
	.maxattr = RESTART_A_MAX,
};

/* cmd line args 
int cmd_line_arg = 0;
module_param(cmd_line_arg, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC( cmd_line_arg, "An integer" ); 
*/

/*
 * restart_genl_notify_dump
 * currently a stub that does nothing
 * could be used to send sent cmds back on dump request
 */
int restart_genl_notify_dump( struct sk_buff *skb, struct netlink_callback *cb )
{
	return 0;
}

int cmd_send(void)
{
	struct sk_buff *skb_out = NULL;
	void *msg_head;
	int rc;
	
	skb_out = genlmsg_new( NLMSG_GOODSIZE, GFP_KERNEL );
	if( skb_out == NULL )
		return -ENOMEM;
	msg_head = genlmsg_put( skb_out, 0, 0, &restart_task_genl_family, 0, 
		RESTART_C_CMD_SEND );
	if( msg_head == NULL )
	{
		return -ENOMEM;
	}

	rc = nla_put_string( skb_out, RESTART_A_MSG, "Hello from kernel space!" );
	if ( rc != 0 )
		return rc;

	genlmsg_end( skb_out, msg_head );
	rc = genlmsg_unicast( &init_net, skb_out, 7114); 
//	rc = genlmsg_multicast( &restart_task_genl_family, skb_out, 0, 0, 0	);
	if ( rc != 0 )
		return rc;

	return 0;
}

int restart_cb( struct notifier_block *nb, unsigned long code, void *_param )
{
	cmd_send();
	return 0;
}

/* init function */
int __init restart_task_init(void)
{
	int rc;
	
	struct nla_policy restart_task_genl_policy[ RESTART_A_MAX + 1] = {

		[RESTART_A_MSG] = { .type = NLA_NUL_STRING },
	};

	struct genl_ops ops[] = {
		{
			.cmd = RESTART_C_CMD_SEND,
			.flags = 0,
			.policy = restart_task_genl_policy,
			.dumpit = restart_genl_notify_dump,
			.doit = NULL,
		},
	};
			
	struct genl_multicast_group grps[] = {
		{
			.name = RESTART_GRP,
		},
	};

	printk( KERN_INFO "OOM_RESTART: initializing\n" );
	
	/* set function to be called by kernel to send cmdline */
	set_cmd_func( cmd_send );



	/* use register with ops and groups */
//	rc = genl_register_family_with_ops_groups( &restart_task_genl_family, ops, grps );
//	if( rc != 0 )
//	{
//		pr_err( "OOM_RESTART: Unable to register netlink family" );
//		return rc;
//	}

	/* a non 0 return means the module cannot be loaded */
	return 0;
}

void __exit restart_task_cleanup(void)
{
	genl_unregister_family( &restart_task_genl_family );
	printk( KERN_INFO "OOM_RESTART: Unloading... \n" );
}

module_init(restart_task_init);
module_exit(restart_task_cleanup);
