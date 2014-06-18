/*! \file LNA.c
    \addtogroup SMART_bias_control

    code to control low noise amplifier (LNA) bias

    \author Urs U. Graf (graf@ph1.uni-koeln.de)

    \date started in 2014

    $Id:$

*/

#include "global.h"
#include "DAC7615.h"


static uint16_t lna_voltage[ CHANNELS_PER_BOARD << 1 ];

/*! set LNA voltage */
void set_lna_voltage( uint8_t A_or_B,
                      int16_t value)
{
  DAC7615_SetOutput( A_or_B + 2, value ); //+2 because the address of the LNAs on the DAC are 2 & 3.
  lna_voltage[A_or_B]=value;
}


/*! get LNA voltage */
uint16_t get_lna_voltage( uint8_t A_or_B)
{
  return( lna_voltage[ A_or_B ] );
}
