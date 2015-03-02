#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "oom_exp.h"

extern int errno;

int alloc( float perc )
{
	int *mem = NULL;
	int c = 1;
 	while(1)
 	{
		mem = malloc( MB(MB_NUM) );
		if( mem == NULL )
		{
			printf( "ERROR: %s.\n", strerror( errno ) );
			printf( "Total memory allocated: %d\n", c * MB_NUM );
			return 1;
		}
		memset( mem, 0, ( int )( MB(MB_NUM) * perc ) );
		c++;
		sleep(1);
 	}
}
