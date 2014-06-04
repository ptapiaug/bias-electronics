/*! \file MC_23K256.c
 * \brief code to access Microchip 23K256 SRAM device via SPI
*/

/*
$Id:$
*/

#include "MC23K256.h"
#include "spi_driver.h"

SPI_Master_t spiMaster_MC23K256;
static uint8_t io_mode;

/*! initialize SPI port for SRAM */
void MC23K256_SPI_Init()
{

  // initialize CS pin as output with wired AND and pull-up.
  MC23K256_SPI_PORT.DIRSET = MC23K256_CS_PIN_bm;
  MC23K256_SPI_PORT.MC23K256_CS_PINCTRL = PORT_OPC_TOTEM_gc;
  // initialize HOLD pin as output with wired AND and pull-up.
  MC23K256_SPI_PORT.DIRSET = MC23K256_HOLD_PIN_bm;
  MC23K256_SPI_PORT.MC23K256_HOLD_PINCTRL = PORT_OPC_TOTEM_gc;

  MC23K256_SPI_PORT.OUTSET = MC23K256_CS_PIN_bm | MC23K256_HOLD_PIN_bm;

  // initialize SPI master
  SPI_MasterInit( &spiMaster_MC23K256, 
                  &MC23K256_SPI_MODULE, 
                  &MC23K256_SPI_PORT, 
                  MC23K256_SPI_LSBFIRST, 
                  MC23K256_SPI_MODE,
                  MC23K256_SPI_INTLVL, 
                  MC23K256_SPI_CLK2X, 
                  MC23K256_SPI_PRESCALER );
}



/*! set io-mode for SRAM */
void MC23K256_SetMode( uint8_t mode )
{
  uint8_t MC23K256_mode[] = { 0x00, 0x80, 0x40 };

  // send status byte
  SPI_MasterSSLow( &MC23K256_SPI_PORT, MC23K256_CS_PIN_bm );
  SPI_MasterTransceiveByte( &spiMaster_MC23K256, MC23K256_WRSR );
  SPI_MasterTransceiveByte( &spiMaster_MC23K256, 
                            MC23K256_mode[mode] | HOLD_DISABLE_bm );
  // RS232_SendChar( MC23K256_mode[mode] | HOLD_DISABLE_bm );
  SPI_MasterSSHigh( &MC23K256_SPI_PORT, MC23K256_CS_PIN_bm );
  io_mode = MC23K256_BYTE_MODE;
}



/*! read status byte from SRAM */
uint8_t MC23K256_ReadStatusByte()
{
  uint8_t ret;

  SPI_MasterSSLow( &MC23K256_SPI_PORT, MC23K256_CS_PIN_bm );
  SPI_MasterTransceiveByte( &spiMaster_MC23K256, MC23K256_RDSR );
  ret = SPI_MasterTransceiveByte( &spiMaster_MC23K256, 0x0 );
  SPI_MasterSSHigh( &MC23K256_SPI_PORT, MC23K256_CS_PIN_bm );

  return( ret );
}



/*! read single byte from SRAM */
uint8_t MC23K256_ReadByte( uint16_t a )
{
  uint8_t ret = 0;

  // slave select
  SPI_MasterSSLow( &MC23K256_SPI_PORT, MC23K256_CS_PIN_bm );
  // send command byte
  SPI_MasterTransceiveByte( &spiMaster_MC23K256, MC23K256_READ );
  // send address
  SPI_MasterTransceiveByte( &spiMaster_MC23K256, a >> 8 );
  SPI_MasterTransceiveByte( &spiMaster_MC23K256, a & 0xff  );
  ret = SPI_MasterTransceiveByte( &spiMaster_MC23K256, 0x00 );
  // unselect slave
  SPI_MasterSSHigh( &MC23K256_SPI_PORT, MC23K256_CS_PIN_bm );

  return( ret );
}



/*! read word from SRAM in byte mode */
uint16_t MC23K256_ReadWord( uint16_t a )
{
  uint16_t ret;

  ret = MC23K256_ReadByte( a++ ) << 8;
  return( ret | MC23K256_ReadByte( a ) );
}




/*! write single byte to address a of SRAM */
void MC23K256_WriteByte( uint16_t a, uint8_t b )
{
  // slave select
  SPI_MasterSSLow( &MC23K256_SPI_PORT, MC23K256_CS_PIN_bm );
  // send command byte
  SPI_MasterTransceiveByte( &spiMaster_MC23K256, MC23K256_WRITE );
  // send address
  SPI_MasterTransceiveByte( &spiMaster_MC23K256, a >> 8 );
  SPI_MasterTransceiveByte( &spiMaster_MC23K256, a & 0xff  );
  SPI_MasterTransceiveByte( &spiMaster_MC23K256, b );
  // unselect slave
  SPI_MasterSSHigh( &MC23K256_SPI_PORT, MC23K256_CS_PIN_bm );
}



/*! write word to address a of SRAM in byte mode */
void MC23K256_WriteWord( uint16_t a, uint16_t w )
{
  MC23K256_WriteByte( a++, w >> 8 );
  MC23K256_WriteByte( a, w & 0xff );
}



/*! start sequential SRAM access (read or write, sequential mode only) */
uint8_t MC23K256_StartSequential( uint16_t a, uint8_t rw )
{
  if ( MC23K256_SEQUENTIAL_MODE != io_mode )
    MC23K256_SetMode( MC23K256_SEQUENTIAL_MODE );

  // slave select
  SPI_MasterSSLow( &MC23K256_SPI_PORT, MC23K256_CS_PIN_bm );
  // send command byte
  SPI_MasterTransceiveByte( &spiMaster_MC23K256, rw );
  // send address
  SPI_MasterTransceiveByte( &spiMaster_MC23K256, a >> 8 );
  SPI_MasterTransceiveByte( &spiMaster_MC23K256, a & 0xff  );

  return( 0 );
}



/*! terminate sequential SRAM access (read or write, sequential mode only),
    leave device in sequential mode */
void MC23K256_StopSequential()
{
  // unselect slave
  SPI_MasterSSHigh( &MC23K256_SPI_PORT, MC23K256_CS_PIN_bm );
}



/*! pull next byte from SRAM (sequential mode only!) */
uint8_t MC23K256_SequentialReadByte()
{
  return( SPI_MasterTransceiveByte( &spiMaster_MC23K256, 0x00 ) );
}



/*! pull next word from SRAM (sequential mode only!) */
uint16_t MC23K256_SequentialReadWord()
{
  uint16_t ret;

  ret = SPI_MasterTransceiveByte( &spiMaster_MC23K256, 0x00 ) << 8;
  return( ret | SPI_MasterTransceiveByte( &spiMaster_MC23K256, 0x00 ) );
}



/*! append byte to SRAM (sequential mode only!) */
void MC23K256_SequentialWriteByte( uint8_t b )
{
  SPI_MasterTransceiveByte( &spiMaster_MC23K256, b );
}



/*! append word to SRAM (sequential mode only!) */
void MC23K256_SequentialWriteWord( uint16_t w )
{
  SPI_MasterTransceiveByte( &spiMaster_MC23K256, w >> 8 );
  SPI_MasterTransceiveByte( &spiMaster_MC23K256, w & 0xff );
}
