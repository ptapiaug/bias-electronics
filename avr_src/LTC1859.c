/*!
  \file LTC1859.c

  \brief code to access ADC LTC1859 device via SPI

  \author U. Graf

  $Id: $
*/

#include "avr_compiler.h"
#include "spi_driver.h"
#include "watchdog.h"
#include "LTC1859.h"

SPI_Master_t spiMaster_LTC1859;

// channel numbering of ADC does not match adresses
uint8_t ADC_addr[] = { 0, 4, 1, 5, 2, 6, 3, 7 };

/*! initialize SPI port for LTC1859 */
void LTC1859_SPI_Init()
{

  // initialize CS pin as output with wired AND and pull-up.
  LTC1859_CS_PORT.DIRSET = LTC1859_CS_PIN_bm;
  LTC1859_CS_PORT.LTC1859_CS_PINCTRL = PORT_OPC_TOTEM_gc;

  // set CS high
  LTC1859_CS_PORT.OUTSET = LTC1859_CS_PIN_bm;

  // define BUSY pin as input
  LTC1859_BUSY_PORT.DIRCLR = LTC1859_BUSY_PIN_bm;

  // initialize SPI master
  SPI_MasterInit( &spiMaster_LTC1859, 
                  &LTC1859_SPI_MODULE, 
                  &LTC1859_SPI_PORT, 
                  LTC1859_SPI_LSBFIRST, 
                  LTC1859_SPI_MODE,
                  LTC1859_SPI_INTLVL, 
                  LTC1859_SPI_CLK2X, 
                  LTC1859_SPI_PRESCALER );
}



/*! read single channel */
LTC1859_DATA_t LTC1859_ReadSingleChannel( uint8_t channel )
{
  LTC1859_DATA_t ret;

  // chip select low
  SPI_MasterSSLow( &LTC1859_CS_PORT, LTC1859_CS_PIN_bm );
  // send command word
  SPI_MasterTransceiveByte( &spiMaster_LTC1859,
                            LTC1859_CMD | ( ADC_addr[ channel & LTC1859_ADDR_MASK ] << LTC1859_ADDR_SHIFT ) );
  SPI_MasterTransceiveByte( &spiMaster_LTC1859, 0x0 );
  // chip select high - starts conversion
  SPI_MasterSSHigh( &LTC1859_CS_PORT, LTC1859_CS_PIN_bm );
  // wait for end of conversion, max. 5 usec
  // while ( !(LTC1859_BUSY_PORT.IN & LTC1859_BUSY_PIN_bm ) );
  delay_us(5);

  // chip select low
  SPI_MasterSSLow( &LTC1859_CS_PORT, LTC1859_CS_PIN_bm );
  // first byte
  ret = SPI_MasterTransceiveByte( &spiMaster_LTC1859, LTC1859_CMD ) << 8;
  // second byte
  ret |= SPI_MasterTransceiveByte( &spiMaster_LTC1859, 0x0 );
  // chip select high
  SPI_MasterSSHigh( &LTC1859_CS_PORT, LTC1859_CS_PIN_bm );
  delay_us( 5 );

  return( ret );
}

void LTC1859_SetConfig( uint8_t channel){

  // chip select low
  SPI_MasterSSLow( &LTC1859_CS_PORT, LTC1859_CS_PIN_bm );
  // send command word
  SPI_MasterTransceiveByte( &spiMaster_LTC1859,
  LTC1859_CMD | ( ADC_addr[ channel & LTC1859_ADDR_MASK ] << LTC1859_ADDR_SHIFT ) );
  // Standart command word. NOT IMPORTANT
  SPI_MasterTransceiveByte( &spiMaster_LTC1859, 0x0 );
  // chip select high - starts conversion
  SPI_MasterSSHigh( &LTC1859_CS_PORT, LTC1859_CS_PIN_bm );
  
}

LTC1859_DATA_t LTC1859_ReadChannel_SetNextConfig( uint8_t channel )
{
  LTC1859_DATA_t ret;
  // wait for end of conversion, max. 5 usec
  // while ( !(LTC1859_BUSY_PORT.IN & LTC1859_BUSY_PIN_bm ) );
  delay_us(5);  // chip select low
  
  SPI_MasterSSLow( &LTC1859_CS_PORT, LTC1859_CS_PIN_bm );
  // send command word
  ret  = SPI_MasterTransceiveByte( &spiMaster_LTC1859,
                                   LTC1859_CMD | ( ADC_addr[ channel & LTC1859_ADDR_MASK ] << LTC1859_ADDR_SHIFT ) ) << 8;
  ret |= SPI_MasterTransceiveByte( &spiMaster_LTC1859, 0x0 );
  // chip select high - starts conversion
  SPI_MasterSSHigh( &LTC1859_CS_PORT, LTC1859_CS_PIN_bm );

  return( ret );
}