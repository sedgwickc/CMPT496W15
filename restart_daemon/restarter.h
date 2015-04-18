#ifndef _RESTARTER_H_
#define _RESTARTER_H_

#define SIZE_CMDLINE 50
#define SIZE_ARG 80
#define MAX_ARGS 20

#define NETLINK_RESTART 30
#define MAX_PAYLOAD 1024

/*******************************
 * open_netlink
 *******************************
 * @param: void
 */
extern int open_netlink();

/*******************************
 * parse_cmdline
 *******************************
 * @cmdline: 
 * @argv
 */
extern int parse_cmdline( char *cmdline, char *argv[] );

/*******************************
 * read_event
 *******************************
 * @sock
 */
extern int read_event(int sock);

/*******************************
 * restart_task
 *******************************
 * @argv_r:
 */
extern int restart_task( char *argv_r[] );
#endif
