// code to activate watchdog timer WDT

#define WDT_IsSyncBusy() ( WDT.STATUS & WDT_SYNCBUSY_bm )

#define WDT_Reset()     __asm__ __volatile__("wdr") //( watchdog_reset( ) )

void watchdog_init( uint8_t per );
