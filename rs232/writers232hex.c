#include "rs232.h"
#include "talk_listen.h"
#include <inttypes.h>

int main (int argc, char * argv[])
{
  speed_t baud=BAUDRATE;
  char str[256]="nix";
  int i;
  uint8_t c;
  int ret;

  ret=initrs232( RS232_DEVICE, baud );

  sscanf( argv[1], "0x%2hhx ", &c );
  sprintf( str, "%c%s\r", c, argv[1]+4 );
  /* for PI Mercury controller */
  sprintf( str, "%c%s", c, argv[1]+4 );
  sprintf( str, "%c%s\n", c, argv[1]+4 );

  ret = writesrs232( str );
  if ( ret )
    printf( "%d bytes written\r", ret );
  else
    printf( "failed\r" );
  fflush( stdout );

  closers232();
return(0);
}
