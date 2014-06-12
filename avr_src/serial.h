/*! \file serial.h
 * \brief header file RS232 communication
*/

/*
$Id:$
*/

#ifndef SERIAL_H
#define SERIAL_H

#define RS232          USARTF0            // Interface
#define RS232_PORT     PORTF              // Port
#define RS232_MISO     PIN2_bm            // Master In, Slave Out
#define RS232_MOSI     PIN3_bm            // Master Out, Slave In
#define RS232_RXC_vect USARTF0_RXC_vect   // interrupt vector
#define RS232_END      10                 // LF terminates commands
// Baudrate: BSEL = 1047, BSCALE = -6  -->  115200 + 0.01%  @ 32MHz
#define RS232_BSEL     1047
#define RS232_BSCALE   -6
// Baudrate: BSEL = 214, BSCALE = -6  -->  115200 - 0.08%  @ 8MHz
// #define RS232_BSEL     214
// #define RS232_BSCALE   -6
// Baudrate: BSEL = 1539, BSCALE = -7  -->  38400 - 0.02%  @ 8MHz
// #define RS232_BSEL     1539
// #define RS232_BSCALE   -7

// 8N1
#define RS232_CHSIZE   USART_CHSIZE_8BIT_gc
#define RS232_PMODE    USART_PMODE_DISABLED_gc
#define RS232_2STOP_BITS false


// prototypes

/*! initialize RS232 port */
void RS232_init( void );

/*! send string via RS232 */
void RS232_SendString( const char *string );

/*! send single character via RS232 */
void RS232_SendChar( const char c );

#endif  // SERIAL_H
