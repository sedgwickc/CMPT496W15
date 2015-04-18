/******************************
 * Charles Sedgwick
 * CMPT496W15
 * This file defines the LKM restart_task used by the kernel to send a command
 * line to a userspace task
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <net/sock.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/skbuff.h>
#include <linux/delay.h>
#include <linux/oom_restart.h>

#define DRIVER_AUTHOR "Charles Sedgwick <sedgwickcharles@gmail.com>"
#define DRIVER_DESC "Sends the cmdline of a task killed by the oom killer to userspace"

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("restart_device");

#define NETLINK_RESTART 30

struct sock *nl_sk = NULL;
int user_pid = 0;

static int cmd_send(char * cmd) {
    struct sk_buff *skb_out;
    struct nlmsghdr *nlh;
    int msg_size;
    int res;

    printk(KERN_INFO "Entering: %s\n", __FUNCTION__);
    msg_size = strlen(cmd);
    skb_out = nlmsg_new(msg_size, 0);

    if (!skb_out) {
        printk(KERN_ERR "Failed to allocate new skb\n");
        return -1;
    }
    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
    NETLINK_CB(skb_out).dst_group = 0; /* not in multicast group */
    strncpy(nlmsg_data(nlh), cmd, msg_size);

    res = nlmsg_unicast(nl_sk, skb_out, user_pid);
    if (res < 0) {
        printk(KERN_INFO "Error while sending to user space. Error id: %d\n", res);
        return res;
    }

    return 0;
}

static void restart_register_pid( struct sk_buff *skb )
{
	
    struct sk_buff *skb_out;
    struct nlmsghdr *nlh;
    int msg_size;
    char *msg = "Hello from kernel";
    int res;

    printk(KERN_INFO "OOM_RESTART: Entering %s\n", __FUNCTION__);
    msg_size = strlen(msg);
    nlh = (struct nlmsghdr *)skb->data;
    printk(KERN_INFO "The kernel received the following payload: %s\n",
    	(char*)nlmsg_data(nlh));
   	user_pid = nlh->nlmsg_pid; /* pid of sending process */

    skb_out = nlmsg_new(msg_size, 0);

    if (!skb_out) {
        printk(KERN_ERR "Failed to allocate new skb\n");
    }
    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
    NETLINK_CB(skb_out).dst_group = 0; /* not in multicast group */
    strncpy(nlmsg_data(nlh), msg, msg_size);

    res = nlmsg_unicast(nl_sk, skb_out, user_pid);

    if (res < 0) {
        printk(KERN_INFO "Error while sending to user space. Error id: %d\n", res);
    }

}

static int __init restart_task_init(void) {

    struct netlink_kernel_cfg cfg = {
    	.input = restart_register_pid,
    };
    printk(KERN_INFO "Starting restart_task module");
    nl_sk = netlink_kernel_create(&init_net, NETLINK_RESTART, &cfg);
    if (!nl_sk) {
        printk(KERN_ALERT "Error creating socket.\n");
        return -10;
    }

	/* set function to be called by kernel to send cmdline */
	set_cmd_func( cmd_send );

    return 0;
}

static void __exit restart_task_exit(void) {

    printk(KERN_INFO "Exiting restart module\n");
    netlink_kernel_release(nl_sk);
}

module_init(restart_task_init);
module_exit(restart_task_exit);
