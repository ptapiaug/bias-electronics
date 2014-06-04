/*! \file MC_23K256.h
 * \brief header file for Microchip 23K256 SRAM device 
*/

/*
$Id:$
*/

#ifndef MC23K256_H
#define MC23K256_H

#include <stdint.h>

// mode of operation
#define MC23K256_BYTE_MODE 0
#define MC23K256_PAGE_MODE 1  // not implemented yet
#define MC23K256_SEQUENTIAL_MODE 2
// uint8_t MC23K256_mode[] = { 0x00, 0x80, 0x40 };

// hold bit
#define HOLD_DISABLE_bm 0x1

// status register values
#define MC23K256_STAT_REG_SEQ 0x41 // sequential mode, HOLD disabled
#define MC23K256_STAT_REG_BYTE 0x01 // byte mode, HOLD disabled
// SRAM commands
#define MC23K256_WRSR 0x1 // write status register
#define MC23K256_RDSR 0x5 // read status register
#define MC23K256_WRITE 0x2 // write data
#define MC23K256_READ 0x3 // read data

// SPI parameter
#define MC23K256_SPI_MODULE      SPIE
#define MC23K256_SPI_PORT        PORTE
#define MC23K256_SPI_LSBFIRST    false
#define MC23K256_SPI_MODE        SPI_MODE_0_gc
#define MC23K256_SPI_INTLVL      SPI_INTLVL_OFF_gc
#define MC23K256_SPI_CLK2X       true
#define MC23K256_SPI_PRESCALER   SPI_PRESCALER_DIV4_gc
#define MC23K256_CS_PORT         PORTE
#define MC23K256_CS_PIN_bm       PIN4_bm
#define MC23K256_CS_PINCTRL      PIN4CTRL
#define MC23K256_HOLD_PORT       PORTE
#define MC23K256_HOLD_PIN_bm     PIN3_bm
#define MC23K256_HOLD_PINCTRL    PIN3CTRL


// prototypes

/*! initialize SPI port for SRAM */
void MC23K256_SPI_Init( void );

/*! set io-mode for SRAM */
void MC23K256_SetMode( uint8_t mode );

/*! read status byte from SRAM */
uint8_t MC23K256_ReadStatusByte( void );

/*! read single byte from SRAM */
uint8_t MC23K256_ReadByte( uint16_t a );

/*! read word from SRAM in byte mode */
uint16_t MC23K256_ReadWord( uint16_t a );

/*! write single byte to address a of SRAM */
void MC23K256_WriteByte( uint16_t a, uint8_t b );

/*! write word to address a of SRAM in byte mode */
void MC23K256_WriteWord( uint16_t a, uint16_t w );

/*! start sequential SRAM access (read or write, sequential mode only) */
uint8_t MC23K256_StartSequential( uint16_t a, uint8_t rw );

/*! terminate sequential SRAM access (read or write, sequential mode only),
    leave device in sequential mode */
void MC23K256_StopSequential( void );

/*! pull next byte from SRAM (sequential mode only!) */
uint8_t MC23K256_SequentialReadByte( void );

/*! pull next word from SRAM (sequential mode only!) */
uint16_t MC23K256_SequentialReadWord( void );

/*! append byte to SRAM (sequential mode only!) */
void MC23K256_SequentialWriteByte( uint8_t b );

/*! append word to SRAM (sequential mode only!) */
void MC23K256_SequentialWriteWord( uint16_t w );

#endif // MC23K256_H
