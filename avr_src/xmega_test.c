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
extern MIXER mixer;

uint8_t debug = false;

volatile uint8_t got_button = 0;
volatile uint8_t button_value = 0;


uint16_t read_int16( volatile char *s )
{
  uint16_t ret = 0;
  uint8_t negative = false;

  if ( debug )
    sprintf( string, "read_int16: %s  ->  ", (char *)s );
  while( isspace( *s ) )
    s++;
  if ( '-' == *s ){
    negative = true;
    s++;
  }
  while ( isdigit( *s ) ){
    ret *= 10;
    ret += *s++ - 48;
  }
  if ( negative )
    ret = (uint16_t)( - ( ret & 0x7FFF ) );
  if ( debug ){
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
  PORTB.DIRSET = 0x0f;
  LED_PORT.DIRSET = 0xFF; // all LED are output

}



int main(void)
{
//####//
  uint8_t mixer_sweep = MIXER_NO_SWEEP;
  
  MIXER_ADDR_WORD_t channel_mask = 0;
  char *cmd_p;
//####//

  uint8_t i;
  char string[ 80 ];

  uint16_t value;
  uint8_t msB, lsB;

  // initialize clock, RS232 and GPIO
  init();
  // initialize DAC
  DAC7615_SPI_Init();
  // initialize ADC
  LTC1859_SPI_Init();
  // initialize SRAM
  MC23K256_SPI_Init();
  // initialize Mixers
  init_mixers();

  //NEW Relay. pin 1 ON.
  _delay_ms(3000);
  PORTB.OUTSET =0x02; 
  
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
        
        case TEST:

          value=0x5060;
          msB= (uint8_t) (value >> 8);
          lsB= (uint8_t) (value);
          RS232_SendChar(lsB);
          RS232_SendChar(msB);          

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

        case GET_LNA_VOLTAGE_A:
            sprintf(string, "LNA_A: %d", get_lna_voltage(LNA_VOLTAGE_A) );
            RS232_SendString( string );
          break;

        case GET_LNA_VOLTAGE_B:
            sprintf(string, "LNA_B: %d", get_lna_voltage(LNA_VOLTAGE_B) );
            RS232_SendString( string );
          break;

        case SET_LNA_VOLTAGE_B:
            set_lna_voltage(LNA_VOLTAGE_B, read_int16(cmd_p));
            sprintf( string, " LNA B Voltage (ch 3) set in %d", read_int16(cmd_p));
            RS232_SendString( string );
            break;

        case MIXER_STATUS:
          sprintf( string,"Mixer Status = %u", 
                  mixer.status);
          RS232_SendString( string );

          sprintf( string,"Mixer Voltage [-16384,16384] = %d. mixer.bias_value [0,4095] = %d", 
                  LTC1859_ReadSingleChannel( LTC1859_MIXER_VOLTAGE),
                  mixer.bias_value);
          RS232_SendString( string );

          sprintf( string,"Mixer Current [-16384,16384] = %d", 
                  LTC1859_ReadSingleChannel( LTC1859_MIXER_CURRENT));
          RS232_SendString( string );
          break;

        case ENABLE_MIXER:
          mixer_enable();
          sprintf( string,
                  "Mixer Enabled. mixer.bias_value = %d, mixer.magnet_value= %d",
                  mixer.bias_value,
                  mixer.magnet_value);
          RS232_SendString( string );
          break;

        case DISABLE_MIXER:
          mixer_disable();
          sprintf( string,
                  "Mixer disabled. mixer.bias_value = %d, mixer.magnet_value= %d",
                  mixer.bias_value,
                  mixer.magnet_value);
          RS232_SendString( string );
          break;

        case START_BIAS_SWEEP:
          mixer_sweep = MIXER_BIAS_SWEEP;
          sprintf( string,
                  "Bias Sweep Enabled");
          RS232_SendString( string );
          break;

        case START_MAGNET_SWEEP:
          mixer_sweep = MIXER_MAGNET_SWEEP;
          sprintf( string,
                  "Magnet Sweep Enabled");
          RS232_SendString( string );
          break;

        case STOP_SWEEP:
          if (mixer_sweep == MIXER_BIAS_SWEEP){
            mixer_bias_restore();
            sprintf( string,
                  "Bias Sweep Disabled");
            RS232_SendString( string );
          }
          if (mixer_sweep == MIXER_MAGNET_SWEEP){
            mixer_magnet_restore();
            sprintf( string,
                  "Magnet Sweep Disabled");
            RS232_SendString( string );
          }
          mixer_sweep = MIXER_NO_SWEEP;
          break;

          case BIAS_SCAN:
            mixer_bias_scan();
            mixer_bias_restore();
            break;
          
          case BIAS_SCAN_TEST:
            // Alive signal: blink onboard LED 3 times
            for ( i = 0; i < 6; i++ )
            {
              ONBOARD_LED_PORT.OUTTGL = ONBOARD_LED;  // toggle LED on/off status
              _delay_ms(1000);
            }
            ONBOARD_LED_PORT.OUTCLR = ONBOARD_LED;  // LED pin low (LED on)
            for ( i = 0; i < 100; i++ )
              {
                mixer_bias_scan();
              }
            break;

          case MAGNET_SCAN:
            mixer_magnet_scan();
            mixer_magnet_restore();
            break;

          case RESET: // Reset
          watchdog_init( WDT_PER_8CLK_gc  ); // set watchdog timer to 8 msec
          while ( true ); // infinite loop to wait for reset
          break;
      }
      // display command number on LEDs
      // LED_PORT.OUTSET = i;
    }
 
  // sweep
  if ( MIXER_BIAS_SWEEP == mixer_sweep )
    mixer_bias_sweep_once();

  if ( MIXER_MAGNET_SWEEP == mixer_sweep )
    mixer_magnet_sweep_once();

  }
}
