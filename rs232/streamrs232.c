#include <stdio.h>
#include <unistd.h>
#include "rs232.h"
#include "talk_listen.h"

#define FALSE 0
#define TRUE 1

int main (int argc, char * argv[])
{
  speed_t baud=BAUDRATE;
  char str;
  int i, cr = FALSE;
  int ret;

  ret=initrs232( RS232_DEVICE, baud );
  
  while( 1 )
  {
    ret = readcrs232( &str );
    if ( ret )
    {
      if ( 0x3 == str )
        continue;
      if ( TRUE == cr )
      {
        if ( '\n' != str )
	  printf( "\n" );
        cr = FALSE;
      }
      printf( "%c", str );
      if ( '\r' == str )
        cr = TRUE;

      fflush( stdout );
    }
    /* usleep( 100000 ); */
  }

  closers232();

  exit(0);
}
