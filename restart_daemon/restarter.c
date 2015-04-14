#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<linux/netlink.h>
#include<sys/types.h>
#include<unistd.h>
#include <errno.h>

#define NETLINK_RESTART 30
#define MAX_PAYLOAD 1024

int open_netlink()
{
        int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_RESTART);
        struct sockaddr_nl src_addr;

        memset((void *)&src_addr, 0, sizeof(src_addr));

        if (sock<0)
        {
        	printf( "Unable to create socket\n" );
        	return sock;
        }
        src_addr.nl_family = AF_NETLINK;
        src_addr.nl_pid = getpid();
       // addr.nl_groups = MYMGRP;
        if (bind(sock,(struct sockaddr *)&src_addr,sizeof(src_addr))<0)
                return sock;
        return sock;
}

int read_event(int sock)
{
        struct sockaddr_nl nladdr;
        struct msghdr msg;
        struct iovec iov[2];
        struct nlmsghdr nlh;
        char buffer[65536];
        int ret;
        
        iov[0].iov_base = (void *)&nlh;
        iov[0].iov_len = sizeof(nlh);
        iov[1].iov_base = (void *)buffer;
        iov[1].iov_len = sizeof(buffer);
        msg.msg_name = (void *)&(nladdr);
        msg.msg_namelen = sizeof(nladdr);
        msg.msg_iov = iov;
        msg.msg_iovlen = sizeof(iov)/sizeof(iov[0]);
        ret=recvmsg(sock, &msg, 0);
        if (ret<0) {
                return ret;
        }
        printf("Received message payload: %s\n", NLMSG_DATA(&nlh));

        return ret;
}

int main(int argc, char *argv[])
{
	int rc;
    int nls = open_netlink();
    struct nlmsghdr *nlh = NULL;
    struct sockaddr_nl dest_addr;
    struct iovec iov;
    struct msghdr msg;

    memset( &dest_addr, 0, sizeof(dest_addr) );
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; /* sending to kernel */
    dest_addr.nl_groups = 0; /* unicast */

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
	nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
	nlh->nlmsg_pid = getpid();
	nlh->nlmsg_flags = 0;

	strcpy( NLMSG_DATA(nlh), "Hello from restarter" );

	memset(&msg, 0, sizeof(msg) );
	iov.iov_base = (void*)nlh;
	iov.iov_len = nlh->nlmsg_len;
	msg.msg_name = (void *)&dest_addr;
	msg.msg_namelen = sizeof(dest_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	printf(" Registering with Kernel\n");
	rc = sendmsg(nls, &msg, 0);
	if( rc < 0 )
	{
        printf( "Error receiving message. ret = %s\n", strerror(errno) );
        return -1;
    }

    rc=recvmsg(nls, &msg, 0);
    if (rc<0) {
        	printf( "Error receiving message. ret = %d\n", rc );
            return -1;
    }
    printf("Received message payload: %s\n", (char *)NLMSG_DATA(nlh));

    if ( nls < 0 ) {
            err(1,"netlink");
    }
	printf( "RESTARTER\n---------\n" );
	printf( "Waiting for message from kernel\n" );



    while (rc >= 0)
    {
        //if( ( rc = read_event(nls)) < 0 )
        if( (rc =  recvmsg(nls, &msg, 0)) < 0) 
        {
        	printf( "Error receiving message. ret = %d\n", rc );
        	return -1;
        }
        else 
        {
        	printf("Received message payload: %s\n", (char *)NLMSG_DATA(nlh));
        }
    }
    printf("recvmsg returned with %d\n", rc);
    close(nls);
    return 0;
}
