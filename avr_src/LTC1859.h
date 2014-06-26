/*!
  \file LTC1859.h

  \brief header file for ADC LTC1859

  \author U. Graf

  $Id: $
*/

#ifndef _LTC1859_H
#define _LTC1859_H

#include <stdint.h>

//Mixer channel address
#define LTC1859_MIXER_VOLTAGE 0x0
#define LTC1859_MIXER_CURRENT 0x1

// ADC parameters
#define LTC1859_SGL  (1<<7) // 0: differential, 1: single ended
#define LTC1859_UNI  (0<<3) // 0: bipolar, 1: unipolar
#define LTC1859_GAIN (0<<2) // 0: (+/-)5 Volts, 1: (+/-)10 Volts

// configure as single ended and bipolar, +/-10 Volts
#define LTC1859_CMD ( LTC1859_SGL | LTC1859_GAIN )
// configure as single ended and bipolar, +/-5 Volts
// #define LTC1859_CMD ( LTC1859_SGL )

#if ( LTC1859_CMD & LTC1859_UNI )
  // unipolar
  typedef uint16_t LTC1859_DATA_t;
  #define LTC1859_FORMAT PRIu16
#else
  // bipolar
  typedef int16_t LTC1859_DATA_t;
  #define LTC1859_FORMAT PRIi16
#endif

#define LTC1859_ADDR_MASK       0x07
#define LTC1859_ADDR_SHIFT      4

#define LTC1859_CURRENT_CHANNEL( i )   (i)
#define LTC1859_POWER_CHANNEL( i )     ((i)+4)

// SPI parameter
#define LTC1859_SPI_MODULE      SPID
#define LTC1859_SPI_PORT        PORTD
#define LTC1859_SPI_STATUS      SPID_STATUS
#define LTC1859_SPI_LSBFIRST    false
#define LTC1859_SPI_MODE        SPI_MODE_0_gc
#define LTC1859_SPI_INTLVL      SPI_INTLVL_OFF_gc
#define LTC1859_SPI_CLK2X       false
#define LTC1859_SPI_PRESCALER   SPI_PRESCALER_DIV4_gc
// CS Pin
#define LTC1859_CS_PORT         PORTD
#define LTC1859_CS_PIN_bm       PIN4_bm
#define LTC1859_CS_PINCTRL      PIN4CTRL
// BUSY Pin
#define LTC1859_BUSY_PORT       PORTD
#define LTC1859_BUSY_PIN_bm     PIN3_bm
#define LTC1859_BUSY_PINCTRL    PIN3CTRL


// prototypes

/*! initialize SPI port for LTC1859 */
void LTC1859_SPI_Init( void );

/*! read single channel */
LTC1859_DATA_t LTC1859_ReadSingleChannel( uint8_t channel );

/* set first configuration for Scan */
void LTC1859_SetConfig( uint8_t channel);

/* Get previous conversion, read actual channel and set the configuration for next conversion*/
LTC1859_DATA_t LTC1859_ReadChannel_SetNextConfig( uint8_t channel );

#endif // _LTC1859_H