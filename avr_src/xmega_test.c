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
//##//
#include "../include/commands.h"
#include "global.h"
#include "hw_status.h"
#include "ctype.h"
#include "string.h"
#include "mixer.h"
#include "LNA.h"
//##//


char string[80];
volatile char cmd[20];
volatile char *cmd_pos = cmd;
volatile uint8_t got_cmd;

uint8_t debug = false;

volatile uint8_t got_button = 0;
volatile uint8_t button_value = 0;


uint16_t read_int16( volatile char *s )
{
  uint16_t ret = 0;
  uint8_t negative = false;

  if ( debug )
  {
    sprintf( string, "read_int16: %s  ->  ", (char *)s );
  }

  while( isspace( *s ) )
    s++;
  if ( '-' == *s )
  {
    negative = true;
    s++;
  }
  while ( isdigit( *s ) )
  {
    ret *= 10;
    ret += *s++ - 48;
  }

  if ( negative )
    ret = (uint16_t)( - ( ret & 0x7FFF ) );

  if ( debug )
  {
    if ( negative )
      sprintf( string + strlen( string), "%"PRIi16"", ret );
    else
      sprintf( string + strlen( string), "%"PRIu16"", ret );
    RS232_SendString( string );
  }

  return( ret );
}


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
  PORTD.DIRSET = 0x6;
  LED_PORT.DIRSET = 0xFF; // all LED are output

}



int main(void)
{
//####//
  MIXER_ADDR_WORD_t channel_mask = 0;
  char *cmd_p;
//####//

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

/////////////////////////////////////////////////
//////////Main Code//////////////////////////////
/////////////////////////////////////////////////

  while( true ){
      
    WDT_Reset(); //watchdog reset
    
    if ( got_cmd ){// command received
      
      got_cmd = false;
      
      channel_mask = cmd[0]; // pointer to command byte
      //(char *) cast to eliminate volatile type
      cmd_p = (char *)cmd + sizeof( MIXER_ADDR_WORD_t ); 
      
      // check command byte. *Read the content of the memory at that adress.
      // ++ increment one size of the data type, this because we want to point at the parameters section.
      switch ( *cmd_p++ ){
        
        default:
        break;
        
        case STATUS: // Version information 0XFE
          sprintf( string, " %s", read_device_id() );
          RS232_SendString( string );
          sprintf( string, " Firmware: %s compiled on %s, %s",
                  __FILE__, __DATE__, __TIME__ );
          RS232_SendString( string );
          sprintf( string, " Version: $Revision:$, $Date:$" );
          RS232_SendString( string );
          break;

        case SET_BIAS_VOLTAGE:
          mixer_bias( read_int16( cmd_p ));
          sprintf( string, " Bias Voltage (ch 0) set in %d", read_int16(cmd_p));
          RS232_SendString( string );
          break;
        
        case SET_MAGNET_CURRENT:
          mixer_magnet( read_int16( cmd_p ));
          sprintf( string, " Magnet Voltage (ch 1) set in %d", read_int16(cmd_p));
          RS232_SendString( string );
          break;

        case SET_LNA_VOLTAGE_A:
            set_lna_voltage(LNA_VOLTAGE_A, read_int16(cmd_p));
            sprintf( string, " LNA A Voltage (ch 2) set in %d", read_int16(cmd_p));
            RS232_SendString( string );
            break;

        case SET_LNA_VOLTAGE_B:
            set_lna_voltage(LNA_VOLTAGE_B, read_int16(cmd_p));
            sprintf( string, " LNA B Voltage (ch 3) set in %d", read_int16(cmd_p));
            RS232_SendString( string );
            break;

        case SET_CHANNEL: // set DAC channel c to value v 0xF0
          // read channel and value from input string
          sscanf( (const char *)cmd + 1, "%hhu %"SCNi16"", &c, &v );
          // set DAC
          // DAC7615_SetOutput( c, v );
          DAC7615_SetOutput( 0, 4095 );
          DAC7615_SetOutput( 1, 4095 );
          DAC7615_SetOutput( 2, 4095 );
          DAC7615_SetOutput( 3, 4095 );
          // write channel and value to output string
          sprintf( string, " bla %d %d", c, v );
          // send string via RS232
          RS232_SendString( string );
          i = 2; // command number
          break;
        case GET_CHANNEL: // read ADC channel c 0xF1
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
        
        case RELAY_ENABLE: //0xF2
          PORTD.OUTSET = 0x2;   //pin 1. OptoFet always ON
          PORTD.OUTSET = 0x4;   //pin 2. Set Gain low 
          PORTA.OUTSET = 0x10;  //pin 4. Enable relay (XOR gate)
          PORTA.OUTCLR = 0x20;  //pin 5. Enable relay (XOR gate)
          PORTD.OUTCLR = 0x4;   //Set gain High again.

          sprintf( string, "Relay enabled");
          RS232_SendString( string );
          break;
        
        case RELAY_DISABLE: //0xF3
          PORTA.OUTSET = 0x10;  //pin 4. Disable relay (XOR gate) 
          PORTA.OUTSET = 0x20;  //pin 5. Disable relay (XOR gate)
          
          sprintf( string, "Relay disabled");
          RS232_SendString( string );
          break;
        
        case RESET: // Reset
          watchdog_init( WDT_PER_8CLK_gc  ); // set watchdog timer to 8 msec
          while ( true ); // infinite loop to wait for reset
          break;
        
        case TEST:
          //set channels voltages

          DAC7615_SetOutput( 1, 1000); //B -0.512
          DAC7615_SetOutput( 2, 2500); //C 0.552
          DAC7615_SetOutput( 3, 3500); //D 1.772
          
          sprintf( string, "B:-0.512, C:0.552, D:1.772C");
          RS232_SendString( string );
          
          //read channels before relay
          sprintf( string, "0 %d", LTC1859_ReadSingleChannel(0)); 
          RS232_SendString( string );

          sprintf( string, "1 %d", LTC1859_ReadSingleChannel(1)); 
          RS232_SendString( string );  

          //enable relays
          PORTA.OUT |= (1 << 4);   // This set 1 in pin4
          PORTA.OUT &= ~(0 << 5);   // This set 0 in pin5
          sprintf( string, "Relay enabled");
          RS232_SendString( string );
          delay_us(10000);

          //read channels before relay
          sprintf( string, "0 %d", LTC1859_ReadSingleChannel(0)); //*5/32768
          RS232_SendString( string );

          sprintf( string, "1 %d", LTC1859_ReadSingleChannel(1)); //*5/32768
          RS232_SendString( string );          

          DAC7615_SetOutput( 0, 500); //A 1.772
          
          sprintf( string, "A:-2.347");
          RS232_SendString( string );

          // //disable relays
          // PORTA.OUT |= (1 << 4);  // This set 1 in pin4
          // PORTA.OUT |= (1 << 5);  // This set 1 in pin5
          // sprintf( string, "Relay disabled");
          // RS232_SendString( string );
          // delay_us(10000);

          // //read channels before relay
          // sprintf( string, "0 %d", LTC1859_ReadSingleChannel(0)); //*5/32768
          // RS232_SendString( string );

          // sprintf( string, "1 %d", LTC1859_ReadSingleChannel(1)); //*5/32768
          // RS232_SendString( string );

          break;

      }
      // display command number on LEDs
      LED_PORT.OUTSET = i;
    }
  }
}
