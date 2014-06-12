/*!
  \file LTC1859.h

  \brief header file for ADC LTC1859

  \author U. Graf

  $Id: $
*/

#ifndef _LTC1859_H
#define _LTC1859_H

#include <stdint.h>

// ADC parameters
#define LTC1859_SGL  (1<<7) // 0: differential, 1: single ended
#define LTC1859_UNI  (0<<3) // 0: bipolar, 1: unipolar
#define LTC1859_GAIN (0<<2) // 0: (+/-)5 Volts, 1: (+/-)10 Volts

#define LTC1859_CMD ( LTC1859_SGL | LTC1859_UNI | LTC1859_GAIN )

// SPI parameter
#define LTC1859_SPI_MODULE      SPID
#define LTC1859_SPI_PORT        PORTD
#define LTC1859_SPI_STATUS      SPID_STATUS
#define LTC1859_SPI_LSBFIRST    false
#define LTC1859_SPI_MODE        SPI_MODE_0_gc
#define LTC1859_SPI_INTLVL      SPI_INTLVL_OFF_gc
#define LTC1859_SPI_CLK2X       true
#define LTC1859_SPI_PRESCALER   SPI_PRESCALER_DIV16_gc
// CS Pin
#define LTC1859_CS_PORT         PORTD
#define LTC1859_CS_PIN_bm       PIN4_bm
#define LTC1859_CS_PINCTRL      PIN4CTRL
// BUSY Pin (not connected on test board)
// #define LTC1859_BUSY_PORT       PORTD
// #define LTC1859_BUSY_PIN_bm     PIN0_bm
// #define LTC1859_BUSY_PINCTRL    PIN0CTRL


// prototypes

/*! initialize SPI port for LTC1859 */
void LTC1859_SPI_Init( void );

/*! read single channel */
uint16_t LTC1859_ReadSingleChannel( uint8_t channel );

#endif // _LTC1859_H
