#include "rs232.h"
#include "talk_listen.h"

int main (int argc, char * argv[])
{
  speed_t baud=BAUDRATE;
  char str[256]="nix";
  int i;
  int ret;

  ret=initrs232( RS232_DEVICE, baud );
  
  sprintf( str, "%s\n", argv[1] );
  printf( "  Writing %s ... ", str );
  sprintf( str, "%s", argv[1] );
  sprintf( str, "%s\r\n", argv[1] );
  sprintf( str, "%s\r", argv[1] );
  sprintf( str, "%s\n", argv[1] );

  ret = writesrs232( str );
  if ( ret )
    printf( "%d bytes written\r", ret );
  else
    printf( "failed\r" );
  fflush( stdout );

  closers232();
return(0);
}
