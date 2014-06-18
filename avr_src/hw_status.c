/*! \file hw_status.c

    Functions to read hardware ID and status of uP

    \author Urs U. Graf (graf@ph1.uni-koeln.de)

    \date started in 2014

    $Id:$
*/

#include <avr/io.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>  
#include <avr_compiler.h>  

volatile uint32_t temperature_cal;

/* read device ID registers */
char *read_device_id( void )
{
  static char id[80];
  char *manufacturer, *device;
  uint16_t device_code;

  switch( MCU_DEVID0 )
  {
    case 0x1E:
      manufacturer = "Atmel";
      break;
    default:
      manufacturer = "unknown";
      break;
  }

  device_code = ( MCU_DEVID1 << 8 ) | MCU_DEVID2;

  switch( device_code )
  {
    case 0x9742:
      device = "ATxmega128A3";
      break;
    case 0x974c:
      device = "ATxmega128A1";
      break;
    default:
      device = "unknown";
      break;
  }

  sprintf( id, "Device ID: 0x%2X%4X, Rev. %c; %s %s", 
           MCU_DEVID0, device_code, MCU_REVID + 65, manufacturer, device );

  return( id );
}


/*! read calibration information from NVM */
uint8_t ReadCalByte( uint8_t index )
{
    uint8_t result;

    /* Load the NVM Command register to read the calibration row. */
    NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;
    result = pgm_read_byte(index);

    /* Clean up NVM Command register. */
    NVM_CMD = NVM_CMD_NO_OPERATION_gc;

    return( result );
} 


/*! setup ADC A to read internal temperature sensor */
void temp_sensor_init( void )
{
  ADCA_CTRLB = ADC_RESOLUTION_12BIT_gc;  // 12 bit resolution
  /* CAUTION: for some reason, the ADC readout depends on the PRESCALER
     value. No idea why!. ADC_PRESCALER_DIV128_gc yields reasonable values */
  ADCA_PRESCALER = ADC_PRESCALER_DIV128_gc;  // ADC clock
  ADCA_REFCTRL = ADC_REFSEL_INT1V_gc | ADC_TEMPREF_bm;
  ADCA_CH0_CTRL = ADC_CH_INPUTMODE_INTERNAL_gc;  // internal signal
  ADCA_CH0_MUXCTRL = ADC_CH_MUXINT_TEMP_gc;  // temperature sensor
  ADCA_CTRLA |= ADC_ENABLE_bm;
  // read ADC calibration from signature row
  ADCA_CAL = ReadCalByte( offsetof(NVM_PROD_SIGNATURES_t, ADCACAL0) ) | 
           ( ReadCalByte( offsetof(NVM_PROD_SIGNATURES_t, ADCACAL1) ) << 8 );
  // read sensor calibration from signature row
  temperature_cal = 35800000 / 
    ( ReadCalByte( offsetof(NVM_PROD_SIGNATURES_t, TEMPSENSE0) ) | 
    ( ReadCalByte( offsetof(NVM_PROD_SIGNATURES_t, TEMPSENSE1) ) << 8 ) );
}


/* read internal temperature sensor */
char *temp_sensor_readout( void )
{
  uint8_t i;
  static char temp_string[25];
  uint16_t temp_val = 0;

  // take 16 readouts for averaging
  for ( i = 0; i < 16; i++ )
  {
    ADCA_CH0_CTRL |= ADC_CH_START_bm;  // start conversion
    while( !ADCA.CH0.INTFLAGS ); // wait for conversion complete
    ADCA.CH0.INTFLAGS = ADC_CH_CHIF_bm;  // clear interrupt flag
    temp_val += ADCA_CH0RES;
  }
  // average
  temp_val >>= 4;

  // calibrate and write to temp-string
  sprintf( temp_string, "Temperature: %7ld",
           ( temp_val - 165 ) * temperature_cal - 27300000 );

  // scale to 1 decimal float
  temp_string[16] = temp_string[15];
  temp_string[15] = '.';
  temp_string[17] = 0;

  return( temp_string );
}