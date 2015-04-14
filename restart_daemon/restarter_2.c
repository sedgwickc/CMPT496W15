#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<linux/netlink.h>
#include<sys/types.h>
#include<unistd.h>

#define NETLINK_RESTART 30
#define MYMGRP 25 //User defined group, consistent in both kernel prog and user prog

int open_netlink()
{
        int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_RESTART);
        struct sockaddr_nl addr;

        memset((void *)&addr, 0, sizeof(addr));

        if (sock<0)
        {
        	printf( "Unable to create socket\n" );
        	return sock;
        }
        addr.nl_family = AF_NETLINK;
        addr.nl_pid = getpid();
        addr.nl_groups = MYMGRP;
        if (bind(sock,(struct sockaddr *)&addr,sizeof(addr))<0)
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
    if ( nls < 0 ) {
            err(1,"netlink");
    }
	printf( "RESTARTER\n---------\n" );
	printf( "Waiting for message from kernel\n" );
    while (1)
    {
        if( ( rc = read_event(nls)) < 0 )
        {
        	printf( "Error receiving message. ret = %d\n", rc );
        	return -1;
        }
    }
        
    return 0;
}
