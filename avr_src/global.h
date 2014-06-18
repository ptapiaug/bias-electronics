/*! \file global.h
    \addtogroup SMART_bias_control

    header file for global definitions

    \author Urs U. Graf (graf@ph1.uni-koeln.de)

    \date started in 2014

    $Id:$
*/

#ifndef _GLOBAL_H
#define _GLOBAL_H

#include <inttypes.h>

#define CHANNELS_PER_BOARD         1
#define MIXER_ADDR_MASK            0x0F  // each bit selects 1 mixer //
#define SINGLE_MIXER_ADDR_MASK     0x03
#define NO_CHANNEL                 0xFF

typedef uint8_t MIXER_ADDR_WORD_t;

#endif // _GLOBAL_H
