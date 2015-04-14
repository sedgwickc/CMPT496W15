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
#define DRIVER_DESC "A test module"

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("test_device");

#define NETLINK_RESTART 30
#define MYGRP 25 //User defined group, consistent in both kernel prog and user prog

struct sock *nl_sk = NULL;

static int cmd_send(void) {
    struct sk_buff *skb_out;
    struct nlmsghdr *nlh;
    int msg_size;
    char *msg = "Hello from kernel";
    int res;

    printk(KERN_INFO "Entering: %s\n", __FUNCTION__);
    msg_size = strlen(msg);
    skb_out = nlmsg_new(msg_size, 0);

    if (!skb_out) {
        printk(KERN_ERR "Failed to allocate new skb\n");
        return -1;
    }
    nlh = nlmsg_put(skb_out, 0, 1, NLMSG_DONE, msg_size, 0);
    //NETLINK_CB(skb_out).dst_group = 1; /* Multicast to group 1, 1<<0 */
    strncpy(nlmsg_data(nlh), msg, msg_size);

    res = nlmsg_multicast(nl_sk, skb_out, 0, MYGRP, 0);
    if (res < 0) {
        printk(KERN_INFO "Error while sending to user space. Error id: %d\n", res);
        return res;
    }

    return 0;
}

static int __init restart_task_init(void) {

    struct netlink_kernel_cfg cfg = {
            .groups = MYGRP,
    };
    printk("Entering: %s\n", __FUNCTION__);
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

    printk(KERN_INFO "exiting hello module\n");
    netlink_kernel_release(nl_sk);
}

module_init(restart_task_init);
module_exit(restart_task_exit);
