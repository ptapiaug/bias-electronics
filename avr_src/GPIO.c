#include "GPIO.h"
#include "avr_compiler.h"

extern volatile uint8_t got_button;
extern volatile uint8_t button_value;

void GPIO_init()
{
  // pushbuttons are inputs
  BUTTON_PORT.DIRSET &= ~BUTTON_MASK;
  
  // pullup for pushbutton pins
  BUTTON_PORT.BUTTON0PINCTRL = PORT_OPC_PULLUP_gc | PORT_ISC_FALLING_gc;
  BUTTON_PORT.BUTTON1PINCTRL = PORT_OPC_PULLUP_gc | PORT_ISC_FALLING_gc;
  BUTTON_PORT.BUTTON2PINCTRL = PORT_OPC_PULLUP_gc | PORT_ISC_FALLING_gc;
  BUTTON_PORT.BUTTON3PINCTRL = PORT_OPC_PULLUP_gc | PORT_ISC_FALLING_gc;

  // enable interrupt for data receive
  BUTTON_PORT.INTCTRL = PORT_INT1LVL_OFF_gc |
                        PORT_INT0LVL_MED_gc;

  // set the pins that can be used as sources for port interrupts 0 & 1
  BUTTON_PORT.INT0MASK = 0xf0;
  // BUTTON_PORT.INT1MASK = 0x40;

  // BUTTON_PORT.INTFLAGS = PORT_INT0IF_bm;
}

ISR( BUTTON_INT0_vect )
{
  delay_us(5000);
  button_value= BUTTON_PORT.IN & BUTTON_MASK;
  if (button_value != 0xf0 ) //0xf0: release button signal
  {
    got_button = true;
  } 
}

