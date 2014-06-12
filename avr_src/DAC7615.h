/*! \file DAC7615.h
 * \brief header file for Burr-Brown DAC7615
*/

/*
$Id:$
*/

#ifndef DAC7615_H
#define DAC7615_H

#include <stdint.h>

// SPI parameter
#define DAC7615_SPI_MODULE        SPIC
#define DAC7615_SPI_PORT          PORTC
#define DAC7615_SPI_LSBFIRST      false
#define DAC7615_SPI_MODE          SPI_MODE_3_gc
#define DAC7615_SPI_INTLVL        SPI_INTLVL_OFF_gc
#define DAC7615_SPI_CLK2X         true
#define DAC7615_SPI_PRESCALER     SPI_PRESCALER_DIV4_gc
// CS Pin
#define DAC7615_CS_PORT           PORTC
#define DAC7615_CS_PIN_bm         PIN4_bm
#define DAC7615_CS_PINCTRL        PIN4CTRL
// RESETSEL Pin
#define DAC7615_RESETSEL_PORT     PORTC
#define DAC7615_RESETSEL_PIN_bm   PIN0_bm
#define DAC7615_RESETSEL_PINCTRL  PIN0CTRL
// RESET Pin
#define DAC7615_RESET_PORT        PORTC
#define DAC7615_RESET_PIN_bm      PIN1_bm
#define DAC7615_RESET_PINCTRL     PIN1CTRL
// LOADREG Pin
#define DAC7615_LOADREG_PORT      PORTC
#define DAC7615_LOADREG_PIN_bm    PIN1_bm
#define DAC7615_LOADREG_PINCTRL   PIN1CTRL
// LOADDACS Pin
#define DAC7615_LOADDACS_PORT     PORTC
#define DAC7615_LOADDACS_PIN_bm   PIN3_bm
#define DAC7615_LOADDACS_PINCTRL  PIN3CTRL

#define DAC7615_MAXVALUE          0x0FFF
#define DAC7615_MIDPOINT          0x0800


// prototypes

/*! initialize SPI port for SRAM */
void DAC7615_SPI_Init( void );

/*! set selected output channel to value */
void DAC7615_SetOutput( uint8_t channel, uint16_t value );

/*! set all output channels */
void DAC7615_SetAllOutputs( uint16_t val_A,
                            uint16_t val_B,
                            uint16_t val_C,
                            uint16_t val_D );

#endif // DAC7615_H
