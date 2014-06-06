#define F_CPU 32000000UL
#include "stdio.h"
#include "inttypes.h"
#include "avr_compiler.h"
#include "serial.h"
#include "GPIO.h"
#include "watchdog.h"
#include "LTC1859.h"
#include "DAC7615.h"
#include "MC23K256.h"


char string[80];
volatile char cmd[20];
volatile char *cmd_pos = cmd;
volatile uint8_t got_cmd;

volatile uint8_t got_button = 0;
volatile uint8_t button_value = 0;

void init( void )
{
  // Clock settings

  // set up internal 32 MHz oscillator
  OSC.CTRL = OSC_RC32MEN_bm;  // enable internal 32 MHz oscillator
  while ( !( OSC_STATUS & OSC_RC32MRDY_bm ) ); // wait to stabilize 32 MHz osc.
  OSC.PLLCTRL = OSC_PLLSRC_RC32M_gc;

  // select clock source
  CCP = 0xD8;   // disable configuration change protection
  CLK.CTRL = CLK_SCLKSEL_RC32M_gc;  // sysclock source: internal 32 MHz osc.

  // enable all 3 interrupt levels
  PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;

  // initialize serial interface
  RS232_init();

  // initialize GPIOS buttons
  GPIO_init();

  // enable interrupts
  sei();


  // Port settings (definitions in GPIO.h)
  ONBOARD_LED_PORT.DIRSET |= ONBOARD_LED;   // LED pin is Output
  ONBOARD_LED_PORT.OUT     = ONBOARD_LED;   // LED pin high (LED off)

  PORTA.DIRSET = 0xff;  // all pins of PORTA are outputs
  LED_PORT.DIRSET = 0xFF; // all LED are output

}



int main(void)
{
  uint8_t i, c;
  int16_t v = 0;
  char string[ 80 ];

  // initialize clock, RS232 and GPIO
  init();
  // initialize DAC
  DAC7615_SPI_Init();
  // initialize ADC
  LTC1859_SPI_Init();
  // initialize SRAM
  MC23K256_SPI_Init();

  // Alive signal: blink onboard LED 3 times
  for ( i = 0; i < 6; i++ )
  {
    ONBOARD_LED_PORT.OUTTGL = ONBOARD_LED;  // toggle LED on/off status
    _delay_ms(100);
  }
  ONBOARD_LED_PORT.OUTCLR = ONBOARD_LED;  // LED pin low (LED on)

  // welcome message
  RS232_SendChar( 'H' );
  RS232_SendChar( 'a' );
  RS232_SendChar( 'l' );
  RS232_SendChar( 'l' );
  RS232_SendChar( 'o' );
  RS232_SendChar( '\n' );

  while( true )
  {
    if( got_button )
    {
      got_button=false;
      switch(BUTTON_PORT.IN & BUTTON_MASK)
      {
        default:
          break;
        case 0x70:
          sprintf( string, "%s compiled %s, %s",
                   __FILE__, __DATE__, __TIME__ );
          RS232_SendString( string );
          
          break;
        case 0Xb0: // set DAC channel c to value v
          // read channel and value from input string
          sscanf( (const char *)cmd + 1, "%hhu %"SCNi16"", &c, &v );
          // set DAC
          DAC7615_SetOutput( 3, 2045 );
          // write channel and value to output string
          sprintf( string, "%d %d", 3, 2045 );
          // send string via RS232
          RS232_SendString( string );
          break;
        case 0Xd0: // read ADC channel c
          
          // write ADC value to output string
          sprintf( string, "%d", LTC1859_ReadSingleChannel( 7 ) );
          // send string via RS232
          RS232_SendString( string );
          break;
        case 0Xe0: // sweep DAC, read value, store in SRAM. When done, dump data
          // show start of measurement on LEDs
          LED_PORT.OUTSET = 0x09;
          // prepare SRAM to store data starting from address 0
          MC23K256_StartSequential( 0, MC23K256_WRITE );
          // sweep up
          for ( v = 0; v <= DAC7615_MAXVALUE; v++ )
          {
            // set new DAC value
            DAC7615_SetOutput( 3, v );
            delay_us(10); // DAC settling time
            // read ADC and store readout in SRAM
            MC23K256_SequentialWriteWord( LTC1859_ReadSingleChannel( 7 ) );
          }
          // sweep down
          for ( v = DAC7615_MAXVALUE; v >= 0; v-- )
          {
            // set new DAC value
            DAC7615_SetOutput( 3, v );
            delay_us(10); // DAC settling time
            // read ADC and store readout in SRAM
            MC23K256_SequentialWriteWord( LTC1859_ReadSingleChannel( 7 ) );
          }
          MC23K256_StopSequential();
          // show end of data transfer on LEDs
          LED_PORT.OUTCLR = 0x09;

          // show start of data transfer on LEDs
          LED_PORT.OUTSET = 0x06;
          // prepare SRAM to read data starting from address 0
          MC23K256_StartSequential( 0, MC23K256_READ );
          for ( v = 0; v <= 2 * DAC7615_MAXVALUE + 1; v++ )
          {
            // read 2 bytes from SRAM and write to string
            sprintf( string, "%d", MC23K256_SequentialReadWord() );
            // send string
            RS232_SendString( string );
          }
          MC23K256_StopSequential();
          // show end of data transfer on LEDs
          LED_PORT.OUTCLR = 0x06;
          break;
      }

    }

    // // check pushbuttons
    // if ( BUTTON_MASK != ( BUTTON_PORT.IN & BUTTON_MASK ) )
    // {
    //   // toggle corresponding LED
    //   LED_PORT.OUTTGL = ~BUTTON_PORT.IN & BUTTON_MASK;
    //   // wait for button to be released
    //   while ( BUTTON_MASK != ( BUTTON_PORT.IN & BUTTON_MASK ) );
    // }

    // check whether we got a command from RS232
    if ( got_cmd )   // command received
    {
      // clear command number display, maintain pushbutton display
      LED_PORT.OUTCLR = (uint8_t)~BUTTON_MASK; // cast avoids compiler warning
      got_cmd = false;
      switch ( cmd[0] )  // check first byte of command string
      {
        default:
          break;
        case 'v': // Version information
          sprintf( string, "%s compiled %s, %s",
                   __FILE__, __DATE__, __TIME__ );
          RS232_SendString( string );
          i = 1; // command number
          break;
        case 'D': // set DAC channel c to value v
          // read channel and value from input string
          sscanf( (const char *)cmd + 1, "%hhu %"SCNi16"", &c, &v );
          // set DAC
          DAC7615_SetOutput( c, v );
          // write channel and value to output string
          sprintf( string, "%d %d", c, v );
          // send string via RS232
          RS232_SendString( string );
          i = 2; // command number
          break;
        case 'a': // read ADC channel c
          // read channel number from input string
          c = atoi( (const char *)cmd + 1 );
          // write ADC value to output string
          sprintf( string, "%d", LTC1859_ReadSingleChannel( c ) );
          // send string via RS232
          RS232_SendString( string );
          i = 3; // command number
          break;
        case 'S': // sweep DAC, read value, store in SRAM. When done, dump data
          // show start of measurement on LEDs
          LED_PORT.OUTSET = 0x09;
          // prepare SRAM to store data starting from address 0
          MC23K256_StartSequential( 0, MC23K256_WRITE );
          // sweep up
          for ( v = 0; v <= DAC7615_MAXVALUE; v++ )
          {
            // set new DAC value
            DAC7615_SetOutput( 3, v );
            delay_us(10); // DAC settling time
            // read ADC and store readout in SRAM
            MC23K256_SequentialWriteWord( LTC1859_ReadSingleChannel( 7 ) );
          }
          // sweep down
          for ( v = DAC7615_MAXVALUE; v >= 0; v-- )
          {
            // set new DAC value
            DAC7615_SetOutput( 3, v );
            delay_us(10); // DAC settling time
            // read ADC and store readout in SRAM
            MC23K256_SequentialWriteWord( LTC1859_ReadSingleChannel( 7 ) );
          }
          MC23K256_StopSequential();
          // show end of data transfer on LEDs
          LED_PORT.OUTCLR = 0x09;

          // show start of data transfer on LEDs
          LED_PORT.OUTSET = 0x06;
          // prepare SRAM to read data starting from address 0
          MC23K256_StartSequential( 0, MC23K256_READ );
          for ( v = 0; v <= 2 * DAC7615_MAXVALUE + 1; v++ )
          {
            // read 2 bytes from SRAM and write to string
            sprintf( string, "%d", MC23K256_SequentialReadWord() );
            // send string
            RS232_SendString( string );
          }
          MC23K256_StopSequential();
          // show end of data transfer on LEDs
          LED_PORT.OUTCLR = 0x06;
          i = 4; // command number
          break;
        case 'R': // Reset
          watchdog_init( WDT_PER_8CLK_gc  ); // set watchdog timer to 8 msec
          while ( true ); // infinite loop to wait for reset
          break;
      }
      // display command number on LEDs
      LED_PORT.OUTSET = i;
    }
  }
}
