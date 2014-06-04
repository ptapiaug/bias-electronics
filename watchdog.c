// code to activate watchdog timer WDT

#include <avr/io.h>
#include <stdlib.h>
#include "avr_compiler.h"
#include "watchdog.h"

/*! activate WDT timer with period per (e.g. WDT_PER_512CLK_gc) */
void watchdog_init( uint8_t per )
{
  per |= WDT_ENABLE_bm | WDT_CEN_bm;
  CCP = CCP_IOREG_gc;  // Configuration change protection
  WDT.CTRL = per;

  while( WDT_IsSyncBusy() );
}
