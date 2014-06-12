/*! \file DAC7615.c
 * \brief code to access Burr-Brown DAC7615 device via SPI
*/

/*
$Id:$
*/

#include "DAC7615.h"
#include "spi_driver.h"

SPI_Master_t spiMaster_DAC7615;

/*! initialize SPI port for SRAM */
void DAC7615_SPI_Init()
{

  // initialize CS pin as output
  DAC7615_CS_PORT.DIRSET = DAC7615_CS_PIN_bm;
  // initialize RESET pin as output
  DAC7615_RESET_PORT.DIRSET = DAC7615_RESET_PIN_bm;
  // initialize RESETSEL pin as output
  DAC7615_RESETSEL_PORT.DIRSET = DAC7615_RESETSEL_PIN_bm;
  // initialize LOADREG pin as output
  DAC7615_LOADREG_PORT.DIRSET = DAC7615_LOADREG_PIN_bm;
  // initialize LOADDACS pin as output
  DAC7615_LOADDACS_PORT.DIRSET = DAC7615_LOADDACS_PIN_bm;

  // set pin values
  DAC7615_CS_PORT.OUTSET = DAC7615_CS_PIN_bm;
  DAC7615_RESET_PORT.OUTSET = DAC7615_RESET_PIN_bm;
  // putting RESETSEL high, resets to midpoint
  DAC7615_RESETSEL_PORT.OUTSET = DAC7615_RESETSEL_PIN_bm;
  DAC7615_LOADREG_PORT.OUTCLR = DAC7615_LOADREG_PIN_bm;
  DAC7615_LOADDACS_PORT.OUTCLR = DAC7615_LOADDACS_PIN_bm;

  //reset
  DAC7615_RESET_PORT.OUTCLR = DAC7615_RESET_PIN_bm;
  delay_us( 10 );
  DAC7615_RESET_PORT.OUTSET = DAC7615_RESET_PIN_bm;

  // initialize SPI master
  SPI_MasterInit( &spiMaster_DAC7615, 
                  &DAC7615_SPI_MODULE, 
                  &DAC7615_SPI_PORT, 
                  DAC7615_SPI_LSBFIRST, 
                  DAC7615_SPI_MODE,
                  DAC7615_SPI_INTLVL, 
                  DAC7615_SPI_CLK2X, 
                  DAC7615_SPI_PRESCALER );

  DAC7615_SetAllOutputs( DAC7615_MIDPOINT,
                         DAC7615_MIDPOINT, 
                         DAC7615_MIDPOINT, 
                         DAC7615_MIDPOINT );
}



// Load output register 
void DAC7615_LoadRegister( uint8_t channel, uint16_t value )
{
  value &= 0xfff;  // blank to 12 bits

  // send control word
  // chip select low
  SPI_MasterSSLow( &DAC7615_SPI_PORT, DAC7615_CS_PIN_bm );
  // send channel and highest 4 bits
  SPI_MasterTransceiveByte( &spiMaster_DAC7615, 
                            ( value >> 8 ) | ( channel << 6 ) );
  // send low 8 bits
  SPI_MasterTransceiveByte( &spiMaster_DAC7615, value );
  // chip select low
  SPI_MasterSSHigh( &DAC7615_SPI_PORT, DAC7615_CS_PIN_bm );
}



/*! set selected output channel to value */
void DAC7615_SetOutput( uint8_t channel, uint16_t value )
{
  // set LOADREG high
  DAC7615_LOADREG_PORT.OUTSET = DAC7615_LOADREG_PIN_bm;

  // load register
  DAC7615_LoadRegister( channel, value );

  // set LOADREG low
  DAC7615_LOADREG_PORT.OUTCLR = DAC7615_LOADREG_PIN_bm;
}



/*! set all output channels */
void DAC7615_SetAllOutputs( uint16_t val_A, 
                            uint16_t val_B, 
                            uint16_t val_C, 
                            uint16_t val_D )
{
  // set LOADREG high
  DAC7615_LOADREG_PORT.OUTSET = DAC7615_LOADREG_PIN_bm;

  // load registers
  DAC7615_LoadRegister( 0, val_A );
  DAC7615_LoadRegister( 1, val_B );
  DAC7615_LoadRegister( 2, val_C );
  DAC7615_LoadRegister( 3, val_D );

  // set LOADREG low
  DAC7615_LOADREG_PORT.OUTCLR = DAC7615_LOADREG_PIN_bm;
}
