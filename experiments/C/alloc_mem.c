/**
 * Charles Sedgwick
 * alloc_all.c
 * Attempts to continually allocate memory until killed by the OOM killer
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "oom_exp.h"

int main( int argc, char** argv )
{
	float p;
	if( argc == 1 )
	{
		printf( "What percentagte of RAM do you want initialized? (0.0-1.0)\n" );
		scanf( "%f", &p );
		alloc( p );
	} 
	else if ( argc == 2 )
	{
		p = strtof( argv[1], NULL );		
		alloc( p );
	}
	return 0;
}

