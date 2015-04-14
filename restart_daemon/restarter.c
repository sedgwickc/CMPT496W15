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

#include <netlink/socket.h>
#include <netlink/netlink.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <linux/genetlink.h>
#include <linux/netlink.h>
#include "restarter.h"

#define GENLMSG_DATA(glh) ((void*)(NLMSG_DATA(glh) + GENL_HDRLEN))
#define GENMSG_PAYLOAD(glh) (NLMSG_PAYLOAD(glh, 0) - GENL_HDRLEN)
#define NLA_DATA(na) ((void *)((char *)(na) + NLA_HDRLEN))
#define RESTART_GRP "RESTART_GRP"

int main ( int argc, char *argv[] )
{
	int rc;
	int nl_fd;
	struct sockaddr_nl nl_address;
	int nl_family_id;
	int nl_rxtx_length;
	struct nlattr *nl_na;
	struct {
		struct nlmsghdr n;
		struct genlmsghdr g;
		char buff[256];
	} nl_request_msg, nl_response_msg;

	printf ( "Hello from Restarter!\n" );

	/* step one: open the socket */
	nl_fd = socket( AF_NETLINK, SOCK_RAW, NETLINK_GENERIC );
	if( nl_fd < 0 )
	{
		perror( "Could not create netlink socket" );
		return -1;
	}
	/* step two: bind the socket */
	memset( &nl_address, 0, sizeof( nl_address ) );
	nl_address.nl_family = AF_NETLINK;
	nl_address.nl_groups = 0;
	if( bind( nl_fd, (struct sockaddr *) &nl_address, sizeof( nl_address )) < 0)
	{
		perror( "Unable to bind netlink socket" );
		return -1;
	}

	rc = nl_socket_add_memberships( (struct nl_sock *) nl_fd, 25 );
	if( rc < 0 )
	{
		printf( " Could not join group. Error # %d\n", rc );
		return -1;
	}

	/* step three: Resolve family ID corresponding to the spring RESTART */
	/* populate header */
	nl_request_msg.n.nlmsg_type = GENL_ID_CTRL;
	nl_request_msg.n.nlmsg_flags = NLM_F_REQUEST;
	nl_request_msg.n.nlmsg_seq = 0;
	nl_request_msg.n.nlmsg_pid = getpid();
	nl_request_msg.n.nlmsg_len = NLMSG_LENGTH(GENL_HDRLEN);
	/* populate the payload's "family header", genlmsghdr in this case */
	nl_request_msg.g.cmd = CTRL_CMD_GETFAMILY;
	nl_request_msg.g.version = 0x1;
	/* populate payload's attributes */
	nl_na = (struct nlattr *) GENLMSG_DATA( &nl_request_msg );
	nl_na->nla_type = CTRL_ATTR_FAMILY_NAME;
	nl_na->nla_len = strlen("RESTART") + 1 + NLA_HDRLEN;
	strcpy( NLA_DATA( nl_na ), "RESTART" );

	nl_request_msg.n.nlmsg_len += NLMSG_ALIGN( nl_na->nla_len );

	memset( &nl_address, 0, sizeof( nl_address ) );
	nl_address.nl_family = AF_NETLINK;

	/* Send the family ID requestmessage to the netlink controller */
	nl_rxtx_length = sendto( nl_fd, (char *) &nl_request_msg,
		nl_request_msg.n.nlmsg_len, 0, (struct sockaddr *) &nl_address, sizeof(
		nl_address) );
	if( nl_rxtx_length != nl_request_msg.n.nlmsg_len )
	{
		perror( "Error sending family ID request message" );
		return -1;
	}

	/* Wait for response */
	nl_rxtx_length = recv( nl_fd, &nl_response_msg, sizeof( nl_response_msg ), 0 );
	if( nl_rxtx_length < 0 )
	{
		perror( "Error receiving family ID request message" );
		return -1;
	}

	if( !NLMSG_OK( (&nl_response_msg.n), nl_rxtx_length ) )
	{
		perror( "Family ID request: invalid message" );
		return -1;
	}

	if( nl_response_msg.n.nlmsg_type == NLMSG_ERROR ) 
	{
		perror( "Family request: receive error" );
		//return -1;
	}

	/* extract family ID */
	nl_na = (struct nlattr *) GENLMSG_DATA( &nl_response_msg );
	nl_na = (struct nlattr *) ((char *) nl_na + NLA_ALIGN(nl_na->nla_len));
	if( nl_na->nla_type ==CTRL_ATTR_FAMILY_ID )
	{
		nl_family_id = *( __u16 * )NLA_DATA( nl_na );
	}

	/* wait for message from kernel */
	printf( "Waiting for message from kernel\n" );
	nl_rxtx_length = recv( nl_fd, &nl_response_msg, sizeof( nl_response_msg ), 0 );
	if( nl_rxtx_length < 0 )
	{
		perror( "Error receiving message from kernel" );
		return -1;
	}

/*
	struct sockaddr_nl src_addr, dest_addr;
	struct nlmsghdr *nlh = NULL;
	struct iovec iov;
	int socket_fd;
	struct msghdr msg;


	socket_fd = socket ( PF_NETLINK, SOCK_RAW, NETLINK_USERSOCK );
	if ( socket_fd < 0 )
	{
		printf ( "ERROR: Unable to open a socket\n" );
		return -1;
	}

	memset( &src_addr, 0, sizeof(src_addr) );
	src_addr.nl_family = AF_NETLINK;
	src_addr.nl_pid = getpid();
	src_addr.nl_groups = 1;

	bind ( socket_fd, (struct sockaddr *)&src_addr, sizeof( src_addr ) );

	memset( &dest_addr, 0, sizeof( dest_addr ) );
	//dest_addr.nl_family = AF_NETLINK;
	//dest_addr.nl_pid = 0;
	//dest_addr.nl_groups = 0;
	

	nlh = (struct nlmsghdr *)malloc( NLMSG_SPACE( SIZE_CMDLINE ) );
	if( nlh == NULL )
	{
		printf( "ERROR: Unable to allocate memory for netlink header.\n" );
		return -1;
	}

	memset( nlh, 0, NLMSG_SPACE( SIZE_CMDLINE ) );
	
	//nlh->nlmsg_len = NLMSG_SPACE( SIZE_CMDLINE );
	//nlh->nlmsg_pid = getpid();
	//nlh->nlmsg_flags = 0;
	

	iov.iov_base = (void *)nlh;
	iov.iov_len = nlh->nlmsg_len;
	msg.msg_name = (void *)&dest_addr;
	msg.msg_namelen = sizeof( dest_addr );
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	printf( "Waiting for message from kernel\n" );
	recvmsg( socket_fd, &msg, 0 );
	printf( "Message from kernel: %s\n", (char *)NLMSG_DATA(nlh) );
	close( socket_fd );

	struct netlink_test *t;
*/
	return 0;

}

