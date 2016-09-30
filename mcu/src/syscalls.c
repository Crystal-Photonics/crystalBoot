/*
 *    Copyright (C) 2015  Crystal-Photonics GmbH
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "board.h"

#include "syscalls.h"
#include "task.h"


#include "serial.h"

/*----------------------------------------------------------------------------
 *        Exported variables
 *----------------------------------------------------------------------------*/

#undef errno
extern int errno ;
extern int  _end ;

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/
extern void _exit( int status ) ;
extern void _kill( int pid, int sig ) ;
extern int _getpid ( void ) ;

extern caddr_t _sbrk ( int incr )
{
	if (incr==0)
		incr=4;
	return (caddr_t) pvPortMalloc(incr);
	/*
	static unsigned char *heap = NULL ;
    unsigned char *prev_heap ;
    if (incr < 0)
    	return 0;
    if ( heap == NULL )
    {
        heap = (unsigned char *)&_end ;
    }
    prev_heap = heap;

    heap += incr ;

    return (caddr_t) prev_heap ;*/
}

extern int link( char *old, char *new )
{
    return -1 ;
}

extern int _close( int file )
{
    return -1 ;
}

extern int _fstat( int file, struct stat *st )
{
    st->st_mode = S_IFCHR ;

    return 0 ;
}

extern int _isatty( int file )
{
    return 1 ;
}

extern int _lseek( int file, int ptr, int dir )
{
    return 0 ;
}

extern int _read(int file, char *ptr, int len)
{
    return 0 ;
}

extern int _write( int file, char *ptr, int len )
{
    int iIndex ;
    

    for ( iIndex=0 ; iIndex < len ; iIndex++, ptr++ )
    {
    	xSerialPutChar(serCOM_DBG,*ptr,0);
    }

    return iIndex ;
}

extern void _exit( int status )
{

	serialSetRTOSRunningFlag(false);
	printf( "Exiting with status %d.\n", status ) ;
    taskDISABLE_INTERRUPTS();

    for ( ; ; ) ;
}

extern void _kill( int pid, int sig )
{
    return ; 
}

extern int _getpid ( void )
{
    return -1 ;
}
