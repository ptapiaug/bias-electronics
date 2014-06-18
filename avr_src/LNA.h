/*! \file LNA.h
    \addtogroup SMART_bias_control

    header file for low noise amplifier (LNA) bias control

    \author Urs U. Graf (graf@ph1.uni-koeln.de)

    \date started in 2014

    $Id:$

*/

#include "stdint.h"

#define LNA_VOLTAGE_A 0
#define LNA_VOLTAGE_B 1

// prototypes

/*! set LNA voltage */
void set_lna_voltage(  	uint8_t A_or_B, 
                        uint16_t value	);

/*! get LNA voltage */
uint16_t get_lna_voltage( uint8_t A_or_B);
