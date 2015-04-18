/*******************************
 * restarter.c
 * Charles Sedgwick
 * CMPT496W15
 *******************************
 * restarter listens on a netlink socket for a cmdline of a task that has been
 * killed by the oom killer and attempts to restart it. It will only attempt a
 * restart if it the was not the last task that it was sent. 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include "logging.h"
#include "restarter.h"

int open_netlink()
{
        int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_RESTART);
        struct sockaddr_nl src_addr;

        memset((void *)&src_addr, 0, sizeof(src_addr));

        if (sock<0)
        {
        	printf( "Unable to create socket\n" );
        	log_write( LOG_ERR, "Unable to create socket");
        	return sock;
        }
        src_addr.nl_family = AF_NETLINK;
        src_addr.nl_pid = getpid();
       // addr.nl_groups = MYMGRP;
        if (bind(sock,(struct sockaddr *)&src_addr,sizeof(src_addr))<0)
        {
        	perror("ERROR: bind ->");
        	log_write( LOG_ERR, "Bind return error" );
            return sock;
        }

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

int parse_cmdline( char *cmdline, char *argv[] )
{
	char *cmd;
	char *tok;
	char *del;
	int i;

	if( cmdline == NULL || argv == NULL )
	{
		printf( "NULL value passed to get_args(char*, char**)" );
		return -1;
	}
	cmd = cmdline;
	del = ";";

	tok = strtok( cmd, del );
	for( i = 0; i < MAX_ARGS; i++ )
	{
		if( tok != NULL )
		{
			argv[i] = malloc( SIZE_ARG + 1 );
			if( argv[i] == NULL )
			{
				printf( "ERROR: nable to allocate memory for argument" );
				log_write( LOG_ERR, "parse_cmdline():Unable to allocate memory for argument");
				for(; i >= 0; i-- )
				{
					free(argv[i]);
				}
				return -1;
			}
			printf( "arg:%s:\n", tok);	
			strncpy( argv[i], tok, SIZE_ARG );
			tok = strtok( NULL, del );
		}
		else 
		{
			argv[i] = NULL;
		}
	}

	return 0;
}

int restart_task( char *argv_r[] )
{
	pid_t child_pid;
	char log_msg[SIZE_ARG + S_LOGMESS];
	memset( log_msg, 0, SIZE_ARG + S_LOGMESS );

	child_pid = fork();
	if( child_pid >= 0 )
	{
		if( child_pid == 0 )
		{
			execvp( argv_r[0], argv_r );
		}
		else
		{
			snprintf( log_msg, SIZE_ARG + S_LOGMESS, "%s restarted with pid %d", 
				argv_r[0], child_pid );
			printf( "%s\n", log_msg);
			log_write( LOG_INFO, log_msg );
		}
	}
	else
	{
		perror("ERROR: fork() failed");
		log_write( LOG_ERR, "fork() failed");
		return -1;
	}

	return child_pid;
}

int main(int argc, char *argv[])
{
	int rc;
    int nls;
    struct nlmsghdr *nlh = NULL;
    struct sockaddr_nl dest_addr;
    struct iovec iov;
    struct msghdr msg;
    char last_cmdline[SIZE_ARG+1];
    memset( last_cmdline, 0, SIZE_ARG+1 );
  
	char log_msg[SIZE_ARG + S_LOGMESS];
	memset( log_msg, 0, SIZE_ARG + S_LOGMESS );

	log_init();

	nls = open_netlink();

	if( nls < 0 )
		return -1;

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
	printf( "RESTARTER\n---------\n" );
	printf(" Registering pid with Kernel\n");
	log_write( LOG_INFO, "Registering pid with kernel" );
	rc = sendmsg(nls, &msg, 0);
	if( rc < 0 )
	{
		snprintf( log_msg, SIZE_ARG + S_LOGMESS, "Error receiving message: %s", 
			strerror(errno) );
        printf( "%s\n", log_msg );
        log_write( LOG_ERR, log_msg );
        return -1;
    }

    rc=recvmsg(nls, &msg, 0);
    if (rc<0) {
		snprintf( log_msg, SIZE_ARG + S_LOGMESS, "Error receiving message: %s", 
			strerror(errno) );
        printf( "%s\n", log_msg );
        log_write( LOG_ERR, log_msg );
        return -1;
    }

	snprintf( log_msg, SIZE_ARG + S_LOGMESS, "Received message payload: %s", 
		(char *)NLMSG_DATA(nlh) );
    printf( "%s\n", log_msg );
    log_write( LOG_INFO, log_msg );

	snprintf( log_msg, SIZE_ARG + S_LOGMESS, "Waiting for command line from kernel.", 
		(char *)NLMSG_DATA(nlh) );
    printf( "%s\n", log_msg );
    log_write( LOG_INFO, log_msg );

	char * argv_r[MAX_ARGS];
    while (rc >= 0)
    {
        if( (rc =  recvmsg(nls, &msg, 0)) < 0) 
        {
			snprintf( log_msg, SIZE_ARG + S_LOGMESS, "Error receiving message: %s", 
				strerror(errno) );
        	printf( "%s\n", log_msg );
        	log_write( LOG_ERR, log_msg );
        	return -1;
        }
        else 
        {
        	printf("Received message payload: %s\n", (char *)NLMSG_DATA(nlh));
        	parse_cmdline( (char *)NLMSG_DATA(nlh), argv_r );
        
        	if( strncmp(last_cmdline, argv_r[0], SIZE_ARG) == 0 )
        	{
				snprintf( log_msg, SIZE_ARG + S_LOGMESS, "%s command line ignored. It was "
					"recently restarted.", argv_r[0] );
    			printf( "%s\n", log_msg );
    			log_write( LOG_WARN, log_msg );
        	}
        	else
        	{
        		if( restart_task( argv_r ) < 0 )
        		{
					snprintf( log_msg, SIZE_ARG + S_LOGMESS, "Could not restart task %s. Trying again...", argv_r[0] );
    				printf( "%s\n", log_msg );
    				log_write( LOG_ERR, log_msg );
					
					sleep(5);
        			if( restart_task( argv_r ) < 0 )
        			{
						snprintf( log_msg, SIZE_ARG + S_LOGMESS, "Could not restart %s after 2 attempts.", argv_r[0] );
    					printf( "%s\n", log_msg );
    					log_write( LOG_ERR, log_msg );
    				}

    			}
        		strncpy(last_cmdline, argv_r[0], SIZE_ARG);
        	}
        }
    }
	snprintf( log_msg, SIZE_ARG + S_LOGMESS, "recvmsg() resturned error %d", rc );
    printf( "%s\n", log_msg );
    log_write( LOG_ERR, log_msg );
    
    log_close();
    close(nls);
    return 0;
}
