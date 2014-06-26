#include "rs232.h"
#include "talk_listen.h"
#include <inttypes.h>
#include <string.h>

int main (int argc, char * argv[])
{
  speed_t baud=BAUDRATE;
  char *p, str[256]="nix";
  int i, a;
  uint8_t c;
  int ret;

  ret=initrs232( RS232_DEVICE, baud );

  p = strstr( argv[1], "0x" );
  if ( !p )
    exit( 1 );
  sscanf( p, "0x%2hhx ", &c );
  *p = 0;
  sscanf( argv[1], "%d", &a );
  sprintf( str, "%c%c%s\n", a, c, p+4 );

  ret = writesrs232( str );
  if ( ret )
    printf( "%d bytes written\r", ret );
  else
    printf( "failed\r" );
  fflush( stdout );

  closers232();
return(0);
}
