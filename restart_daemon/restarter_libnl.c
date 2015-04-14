/* Charles Sedgwick
 * CMPT 496 W15
 * restart.c
 * Receives request and data from kernel to restart a task that has been
 * recently killed by the oom killer.
 * When restart recieves a task to be restarted, it forks then execs task in the
 * child. The parent continues to listen for future tasks to restart. 
 * 
 * Restarter receives task to restart 
 * > using netlink?
 * > proc?
 *
 * Restarter requires the cmdline used to launch a task
 * > if the parameters reference files, absolute paths must be available or path
 * the task was executed from must be known
 * > the user or set of environment variables the task started with
 * -> could create the requirement that any task started must share the same
 * environment as the restarter in order to make this a non issue.
 */

#include <netlink/netlink.h>
#include <netlink/socket.h>
#include <netlink/msg.h>
//#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//#include <linux/genetlink.h>
//#include <linux/netlink.h>
#include "restarter.h"

#define GENLMSG_DATA(glh) ((void*)(NLMSG_DATA(glh) + GENL_HDRLEN))
#define GENMSG_PAYLOAD(glh) (NLMSG_PAYLOAD(glh, 0) - GENL_HDRLEN)
#define NLA_DATA(na) ((void *)((char *)(na) + NLA_HDRLEN))
#define RESTART_GRP "RESTART_GRP"

static int my_cb( struct nl_msg *msg, void * arg )
{
	return 0;
}

int main ( int argc, char *argv[] )
{
	struct nl_sock *sk;

	printf( "RESTARTER\n---------\n" );

	sk = nl_socket_alloc();
	if( sk == NULL )
	{
		printf( "Error allocating new socket\n" );
		return -1;
	}

	nl_socket_disable_seq_check( sk );
	
	nl_socket_modify_cb( sk, NL_CB_VALID, NL_CB_CUSTOM, my_cb, NULL );

	nl_connect( sk, NETLINK_GENERIC );

	nl_socket_add_memberships( sk, NETLINK_GENERIC, 0 );

	printf( "Waiting for message from kernel\n" );
	while(1)
		nl_recvmsgs_default( sk );
	
	return 0;

}

