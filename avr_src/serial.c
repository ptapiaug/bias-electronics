/*! \file serial.c
 * \brief code for the RS232 communication
*/

/*
$Id:$
*/

#include "usart_driver.h"
#include "serial.h"


extern volatile char cmd[];
extern volatile char *cmd_pos;
extern volatile uint8_t got_cmd;


/*! initialize RS232 port */
void RS232_init()
{
  RS232_PORT.DIRSET = RS232_MOSI; // MOSI is output
  RS232_PORT.DIRCLR = RS232_MISO; // MISO is input
  // data format
  USART_Format_Set( &RS232, RS232_CHSIZE, RS232_PMODE, RS232_2STOP_BITS );
  // baud rate
  USART_Baudrate_Set( &RS232, RS232_BSEL , RS232_BSCALE );
  // enable receive
  USART_Rx_Enable( &RS232 );
  // enable transmit
  USART_Tx_Enable( &RS232 );

  // enable interrupt for data receive
  RS232.CTRLA = USART_RXCINTLVL_MED_gc |
                USART_TXCINTLVL_OFF_gc |
                USART_DREINTLVL_OFF_gc;
}



/*! send string via RS232 */
void RS232_SendString( const char *string )
{
  const char *p = string;

  while ( *p )
    RS232_SendChar( *p++ ); // Send one char
  RS232_SendChar( RS232_END ); // Send end of line character
}



/*! send single character via RS232 */
void RS232_SendChar( const char c ) // If the Size is bigger, this send the less significant byte.
{
  /* Send one char. */
  do{
  }while( !USART_IsTXDataRegisterEmpty( &RS232 ) );
    USART_PutChar( &RS232, c );
}



/* Interrupt service routine for RS232 */
ISR( RS232_RXC_vect )
{
  *cmd_pos = RS232.DATA; // load data byte
  if ( RS232_END == *cmd_pos ) // end of line character received?
  {
    *cmd_pos = 0;    // terminate command string
    cmd_pos = cmd;   // set pointer back to start of command string
    got_cmd = true;  // set command flag
  }
  else
    cmd_pos++;       // increment pointer
}
