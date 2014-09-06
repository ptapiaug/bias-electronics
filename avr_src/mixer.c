/*! \file mixer.c
    \addtogroup SMART_bias_control

    Code for high level mixer functions

    \author Urs U. Graf (graf@ph1.uni-koeln.de)

    \date started in 2014

    $Id:$
*/

#include "avr_compiler.h"
#include "global.h"
#include "serial.h"
#include "mixer.h"
#include "GPIO.h"
#include "LTC1859.h"
#include "watchdog.h"

#include "ctype.h"
#include "string.h"
#include "stdio.h"
MIXER mixer;

/*! initialize mixers */
void init_mixers( void )
{
    mixer.status = 0;
    mixer_disable();
    mixer_bias( MIXER_BIAS_ZERO );
    mixer_magnet( MIXER_MAGNET_ZERO );
}



/*! set mixer bias voltage */
void mixer_bias( int16_t v )
{
  mixer.bias_value = v;
  DAC7615_SetOutput( DAC7615_BIAS_CHANNEL, v );
  mixer.status &= ~MIXER_BIAS_DETUNED; // unset DETUNED bit
}

/*! restore bias voltage values (e.g. after sweep) */
void mixer_bias_restore( void )
{
  if ( mixer.status & MIXER_BIAS_DETUNED )
    mixer_bias( mixer.bias_value );
}

/*! set mixer magnetic field */
void mixer_magnet(int16_t v )
{
  mixer.magnet_value = v;
  DAC7615_SetOutput( DAC7615_MAGNET_CHANNEL, v );
  mixer.status &= ~MIXER_MAGNET_DETUNED; // unset DETUNED bit
}

/*! restore magnetic field values (e.g. after sweep) */
void mixer_magnet_restore( void )
{
	if ( mixer.status & MIXER_MAGNET_DETUNED )
    mixer_magnet(mixer.magnet_value );
}

// void mixer_enable( void )
// {
//   PORTD.OUTSET = 0x2;   //pin 1. OptoFet always ON

//   //Set Dac to 0 before conect to the mixer
//   DAC7615_SetOutput( DAC7615_BIAS_CHANNEL, MIXER_BIAS_ZERO ); 
//   DAC7615_SetOutput( DAC7615_MAGNET_CHANNEL, MIXER_MAGNET_ZERO );
//   //pin 2. Set Gain low  
//   PORTD.OUTSET = 0x4; 
//   //Enable the relays (XOR gate)
//   PORTA.OUTSET = 0x10;  //pin 4.
//   PORTA.OUTCLR = 0x20;  //pin 5.
//   delay_us(5000); //wait for the relays.
//   //pin2. Set gain high again.
//   PORTD.OUTCLR = 0x4;   
//   //Set the DAC with the stored values.
//   DAC7615_SetOutput( DAC7615_BIAS_CHANNEL, mixer.bias_value ); 
//   DAC7615_SetOutput( DAC7615_MAGNET_CHANNEL, mixer.magnet_value ); 
//   mixer.status &= ~MIXER_DISABLED;  // unset DISABLED bit
// }

void mixer_enable( void ) //Bernhard version
{
  PORTD.OUTSET = 0x2;   //pin 1. OptoFet always ON

  //Set Dac to 0 before conect to the mixer
  DAC7615_SetOutput( DAC7615_BIAS_CHANNEL, MIXER_BIAS_ZERO ); 
  DAC7615_SetOutput( DAC7615_MAGNET_CHANNEL, MIXER_MAGNET_ZERO );
  //pin 2. Set Gain low  
  PORTD.OUTSET = 0x4; 
  //Enable the relays (XOR gate)
  PORTA.OUTSET = 0x40;  //pin 6.
  _delay_ms(20); //wait for the relays.
  //pin2. Set gain high again.
  PORTD.OUTCLR = 0x4;   
  //Set the DAC with the stored values.
  DAC7615_SetOutput( DAC7615_BIAS_CHANNEL, mixer.bias_value ); 
  DAC7615_SetOutput( DAC7615_MAGNET_CHANNEL, mixer.magnet_value ); 
  mixer.status &= ~MIXER_DISABLED;  // unset DISABLED bit
}

// void mixer_disable( void)
// {
//   DAC7615_SetOutput( DAC7615_BIAS_CHANNEL, MIXER_BIAS_ZERO ); 
//   DAC7615_SetOutput( DAC7615_MAGNET_CHANNEL, MIXER_MAGNET_ZERO ); 
//   PORTD.OUTSET = 0x4;   //pin 2. Set Gain low 
//   PORTA.OUTSET = 0x10;  //pin 4. Disable relay (XOR gate)
//   PORTA.OUTSET = 0x20;  //pin 5. Disable relay (XOR gate)
//   mixer.status |= MIXER_DISABLED;  // set DISABLED bit
// }

void mixer_disable( void) // bernhard version
{
  DAC7615_SetOutput( DAC7615_BIAS_CHANNEL, MIXER_BIAS_ZERO ); 
  DAC7615_SetOutput( DAC7615_MAGNET_CHANNEL, MIXER_MAGNET_ZERO ); 
  PORTD.OUTSET = 0x4;   //pin 2. Set Gain low 
  _delay_ms(5);
  PORTA.OUTCLR = 0x40;  //pin 6. Disable relay (XOR gate)
 
  mixer.status |= MIXER_DISABLED;  // set DISABLED bit
}

/*! sweep mixer bias up and down */
void mixer_bias_sweep_once( void )
{
  int16_t i;
  // sweep only if mixer is enabled
  if ( mixer.status & MIXER_DISABLED )
    return;  

  mixer.status |= MIXER_BIAS_DETUNED; // set DETUNED bit

  for ( i  = MIXER_BIAS_MIN;
        i  < MIXER_BIAS_MAX;
        i += MIXER_BIAS_SWEEP_STEP ){
    DAC7615_SetOutput( DAC7615_BIAS_CHANNEL, i );
    delay_us(10);
    }

  for ( i  = MIXER_BIAS_MAX;
        i >= MIXER_BIAS_MIN;
        i -= MIXER_BIAS_SWEEP_STEP )
    {
    DAC7615_SetOutput( DAC7615_BIAS_CHANNEL, i );
    delay_us(10);
    }
}


/*! sweep mixer magnet current up and down */
void mixer_magnet_sweep_once( void )
{
  int16_t i;

  // sweep only, if mixer is enabled
  if ( mixer.status & MIXER_DISABLED )
    return;  

  mixer.status |= MIXER_MAGNET_DETUNED; // set DETUNED bit

  for ( i  = MIXER_MAGNET_MIN;
        i  < MIXER_MAGNET_MAX;
        i += MIXER_MAGNET_SWEEP_STEP )
    DAC7615_SetOutput( DAC7615_MAGNET_CHANNEL, i);

  for ( i  = MIXER_MAGNET_MAX;
        i >= MIXER_MAGNET_MIN;
        i -= MIXER_MAGNET_SWEEP_STEP )
    DAC7615_SetOutput( DAC7615_MAGNET_CHANNEL, i);
}

void mixer_bias_scan(void){

  int16_t i;
  int8_t channel1, channel2;
  
  uint16_t twoBytesValue;
  uint8_t mostSignificantByte, lessSignificantByte;
  
  // sweep only if mixer is enabled
  if ( mixer.status & MIXER_DISABLED )
    return; 

  // Alive signal: blink onboard LED 3 times
  // for ( i = 0; i < 8; i++ )
  // {
  //   ONBOARD_LED_PORT.OUTTGL = ONBOARD_LED;  // toggle LED on/off status
  //   _delay_ms(100);
  // }
  // ONBOARD_LED_PORT.OUTCLR = ONBOARD_LED;  // LED pin low (LED on)

  //Address of channels
  channel1 = LTC1859_MIXER_VOLTAGE;
  channel2 = LTC1859_MIXER_CURRENT;
  
  mixer.status |= MIXER_BIAS_DETUNED; // set DETUNED bit

  //////////////////////////////////////////////////////
  /////////////// FIRST ITERATION //////////////////////
  // Set the DAC in the minimum value
  DAC7615_SetOutput( DAC7615_BIAS_CHANNEL, MIXER_BIAS_MIN );
  
  _delay_ms(20);
  //_delay_ms(100);

  // Set up the config for reading of the channel1. Store the analog value of chn1.
  LTC1859_SetConfig(channel1);

  //Read the conversion of the stored analog value with previously loaded config of chn1.
  twoBytesValue = LTC1859_ReadChannel_SetNextConfig(channel2);
    mostSignificantByte = (uint8_t) ( twoBytesValue >> 8 );
    lessSignificantByte = (uint8_t) ( twoBytesValue );
  //Send the less significant byte first, for a correct interpretation.

  RS232_SendChar(lessSignificantByte);
  RS232_SendChar(mostSignificantByte);



  /////////////////////////////////////////////////////
  //////////////START SCAN LOOP////////////////////////

  for ( i  = MIXER_BIAS_MIN + 1 ;//+1 Because the first iteration was realized previously.
        i  < MIXER_BIAS_MAX;
        i += MIXER_BIAS_SWEEP_STEP ){ 
    
    DAC7615_SetOutput( DAC7615_BIAS_CHANNEL, i );

    twoBytesValue = LTC1859_ReadChannel_SetNextConfig(channel1);
    mostSignificantByte = (uint8_t) ( twoBytesValue >> 8 );
    lessSignificantByte = (uint8_t) ( twoBytesValue );
    RS232_SendChar(lessSignificantByte);
    RS232_SendChar(mostSignificantByte);

    twoBytesValue = LTC1859_ReadChannel_SetNextConfig(channel2);
    mostSignificantByte = (uint8_t) ( twoBytesValue >> 8 );
    lessSignificantByte = (uint8_t) ( twoBytesValue );
    RS232_SendChar(lessSignificantByte);
    RS232_SendChar(mostSignificantByte);

  }

  for ( i  = MIXER_BIAS_MAX;
        i >= MIXER_BIAS_MIN;
        i -= MIXER_BIAS_SWEEP_STEP ){

    DAC7615_SetOutput( DAC7615_BIAS_CHANNEL, i );

    twoBytesValue = LTC1859_ReadChannel_SetNextConfig(channel1);
    mostSignificantByte = (uint8_t) ( twoBytesValue >> 8 );
    lessSignificantByte = (uint8_t) ( twoBytesValue );
    RS232_SendChar(lessSignificantByte);
    RS232_SendChar(mostSignificantByte);

    twoBytesValue = LTC1859_ReadChannel_SetNextConfig(channel2);
    mostSignificantByte = (uint8_t) ( twoBytesValue >> 8 );
    lessSignificantByte = (uint8_t) ( twoBytesValue );
    RS232_SendChar(lessSignificantByte);
    RS232_SendChar(mostSignificantByte);

  }

  //Read the last value from ch2. The argument of the function is not important.
  twoBytesValue = LTC1859_ReadChannel_SetNextConfig(channel1);
    mostSignificantByte = (uint8_t) ( twoBytesValue >> 8 );
    lessSignificantByte = (uint8_t) ( twoBytesValue );
    RS232_SendChar(lessSignificantByte);
    RS232_SendChar(mostSignificantByte);  

  // Alive signal: blink onboard LED 3 times
  // for ( i = 0; i < 8; i++ ){
  //   ONBOARD_LED_PORT.OUTTGL = ONBOARD_LED;  // toggle LED on/off status
  //   _delay_ms(100);
  // }
  // ONBOARD_LED_PORT.OUTCLR = ONBOARD_LED;  // LED pin low (LED on)

}

void mixer_magnet_scan(void){

  int16_t i;
  int8_t channel1, channel2;
  
  uint16_t twoBytesValue;
  uint8_t mostSignificantByte, lessSignificantByte;
  
  // sweep only if mixer is enabled
  if ( mixer.status & MIXER_DISABLED )
    return; 
  //Address of channels
  channel1 = LTC1859_MIXER_VOLTAGE;
  channel2 = LTC1859_MIXER_CURRENT;
  
  mixer.status |= MIXER_MAGNET_DETUNED; // set DETUNED bit

  //////////////////////////////////////////////////////
  /////////////// FIRST ITERATION //////////////////////
  // Set the DAC in the minimum value
  DAC7615_SetOutput( DAC7615_MAGNET_CHANNEL, MIXER_MAGNET_MIN );
  
  _delay_ms(20);

  // Set up the config for reading of the channel1. Store the analog value of chn1.
  LTC1859_SetConfig(channel1);

  //Read the conversion of the stored analog value with previously loaded config of chn1.
  twoBytesValue = LTC1859_ReadChannel_SetNextConfig(channel2);
    mostSignificantByte = (uint8_t) ( twoBytesValue >> 8 );
    lessSignificantByte = (uint8_t) ( twoBytesValue );
  //Send the less significant byte first, for a correct interpretation.

  RS232_SendChar(lessSignificantByte);
  RS232_SendChar(mostSignificantByte);



  /////////////////////////////////////////////////////
  //////////////START SCAN LOOP////////////////////////

  for ( i  = MIXER_MAGNET_MIN + 1 ;//+1 Because the first iteration was realized previously.
        i  < MIXER_MAGNET_MAX;
        i += MIXER_MAGNET_SWEEP_STEP ){ 
    
    DAC7615_SetOutput( DAC7615_MAGNET_CHANNEL, i );

    twoBytesValue = LTC1859_ReadChannel_SetNextConfig(channel1);
    mostSignificantByte = (uint8_t) ( twoBytesValue >> 8 );
    lessSignificantByte = (uint8_t) ( twoBytesValue );
    RS232_SendChar(lessSignificantByte);
    RS232_SendChar(mostSignificantByte);

    twoBytesValue = LTC1859_ReadChannel_SetNextConfig(channel2);
    mostSignificantByte = (uint8_t) ( twoBytesValue >> 8 );
    lessSignificantByte = (uint8_t) ( twoBytesValue );
    RS232_SendChar(lessSignificantByte);
    RS232_SendChar(mostSignificantByte);

  }

  for ( i  = MIXER_MAGNET_MAX;
        i >= MIXER_MAGNET_MIN;
        i -= MIXER_MAGNET_SWEEP_STEP ){

    DAC7615_SetOutput( DAC7615_MAGNET_CHANNEL, i );

    twoBytesValue = LTC1859_ReadChannel_SetNextConfig(channel1);
    mostSignificantByte = (uint8_t) ( twoBytesValue >> 8 );
    lessSignificantByte = (uint8_t) ( twoBytesValue );
    RS232_SendChar(lessSignificantByte);
    RS232_SendChar(mostSignificantByte);

    twoBytesValue = LTC1859_ReadChannel_SetNextConfig(channel2);
    mostSignificantByte = (uint8_t) ( twoBytesValue >> 8 );
    lessSignificantByte = (uint8_t) ( twoBytesValue );
    RS232_SendChar(lessSignificantByte);
    RS232_SendChar(mostSignificantByte);

  }

  //Read the last value from ch2. The argument of the function is not important.
  twoBytesValue = LTC1859_ReadChannel_SetNextConfig(channel1);
    mostSignificantByte = (uint8_t) ( twoBytesValue >> 8 );
    lessSignificantByte = (uint8_t) ( twoBytesValue );
    RS232_SendChar(lessSignificantByte);
    RS232_SendChar(mostSignificantByte);  


}


// void mixer_magnet_scan(void){

//   int16_t i;
//   // sweep only if mixer is enabled
//   if ( mixer.status & MIXER_DISABLED )
//     return; 

//   // Alive signal: blink onboard LED 3 times
//   for ( i = 0; i < 8; i++ )
//   {
//     ONBOARD_LED_PORT.OUTTGL = ONBOARD_LED;  // toggle LED on/off status
//     _delay_ms(100);
//   }
//   ONBOARD_LED_PORT.OUTCLR = ONBOARD_LED;  // LED pin low (LED on)


//   int8_t channel1, channel2;
  
//   //Address of channels
//   channel1 = LTC1859_MIXER_VOLTAGE;
//   channel2 = LTC1859_MIXER_CURRENT;
  
//   mixer.status |= MIXER_MAGNET_DETUNED; // set DETUNED bit

//   //First Iteration
//   DAC7615_SetOutput( DAC7615_MAGNET_CHANNEL, MIXER_MAGNET_MIN );
//   LTC1859_SetConfig(channel1);
//   RS232_SendChar( LTC1859_ReadChannel_SetNextConfig(channel2) );

//   for ( i  = MIXER_MAGNET_MIN + 1 ; //+1 Because the first iteration was realized previously.
//         i  < MIXER_MAGNET_MAX;
//         i += MIXER_MAGNET_SWEEP_STEP ){ 
    
//     DAC7615_SetOutput( DAC7615_MAGNET_CHANNEL, i );
//     //delay_us(5);
//     RS232_SendChar( LTC1859_ReadChannel_SetNextConfig(channel1) );
//     RS232_SendChar( LTC1859_ReadChannel_SetNextConfig(channel2) );
  
//   }

//   for ( i  = MIXER_MAGNET_MAX;
//         i >= MIXER_MAGNET_MIN;
//         i -= MIXER_MAGNET_SWEEP_STEP ){

//     DAC7615_SetOutput( DAC7615_MAGNET_CHANNEL, i );
//     //delay_us(5);
//     RS232_SendChar( LTC1859_ReadChannel_SetNextConfig(channel1) );
//     RS232_SendChar( LTC1859_ReadChannel_SetNextConfig(channel2) );

//   }

//   // Alive signal: blink onboard LED 3 times
//   for ( i = 0; i < 8; i++ )
//   {
//     ONBOARD_LED_PORT.OUTTGL = ONBOARD_LED;  // toggle LED on/off status
//     _delay_ms(100);
//   }
//   ONBOARD_LED_PORT.OUTCLR = ONBOARD_LED;  // LED pin low (LED on)

// }