/* header file for general purpose IO port and pin definitions */

#ifndef _GPIO_H
#define _GPIO_H

// Port definitions

// LED on CRUMB board
#define ONBOARD_LED_PORT PORTD
#define ONBOARD_LED      PIN0_bm

// 8 green LEDs
#define LED_PORT         PORTB

// pushbuttons
#define BUTTON_PORT      PORTF
#define BUTTON0PINCTRL   PIN4CTRL
#define BUTTON1PINCTRL   PIN5CTRL
#define BUTTON2PINCTRL   PIN6CTRL
#define BUTTON3PINCTRL   PIN7CTRL
#define BUTTON_MASK      0xF0

#endif // _GPIO_H
