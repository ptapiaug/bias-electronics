/*! \file mixer.c
    \addtogroup SMART_bias_control

    Code for high level mixer functions

    \author Urs U. Graf (graf@ph1.uni-koeln.de)

    \date started in 2014

    $Id:$
*/

#include "avr_compiler.h"
#include "global.h"
#include "serial.h"
#include "mixer.h"
#include "GPIO.h"
#include "LTC1859.h"
#include "watchdog.h"

MIXER mixer;


// /*! initialize mixers */
// void init_mixers( void )
// {
//   uint8_t i;

//   for ( i = 0; i < CHANNELS_PER_BOARD; i++ )
//   {
//     mixer[ i ].status = 0;
//     mixer_disable( i );
//     mixer_bias( i, MIXER_BIAS_ZERO );
//     mixer_magnet( i, MIXER_MAGNET_ZERO );
//   }
// }


/*! set mixer bias voltage */
void mixer_bias( int16_t v )
{
  mixer.bias_value = v;
  DAC7615_SetOutput( DAC7615_BIAS_CHANNEL, v );
  mixer.status &= ~MIXER_BIAS_DETUNED; // unset DETUNED bit
}

/*! restore bias voltage values (e.g. after sweep) */
void mixer_bias_restore( void )
{
  if ( mixer.status & MIXER_BIAS_DETUNED )
    mixer_bias( mixer.bias_value );
}

/*! set mixer magnetic field */
void mixer_magnet(int16_t v )
{
  mixer.magnet_value = v;
  DAC7615_SetOutput( DAC7615_MAGNET_CHANNEL, v );
  mixer.status &= ~MIXER_MAGNET_DETUNED; // unset DETUNED bit
}

/*! restore magnetic field values (e.g. after sweep) */
void mixer_magnet_restore( void )
{
	if ( mixer.status & MIXER_MAGNET_DETUNED )
    mixer_magnet(mixer.magnet_value );
}


// /*! sweep mixer bias up and down */
// void mixer_bias_sweep_once( void )
// {
//   int16_t i;

//   if ( NO_CHANNEL == selected_mixer )
//     return;

//   if ( mixer[ selected_mixer ].status & MIXER_DISABLED )
//     return;  // sweep only, if mixer is enabled

//   mixer[ selected_mixer ].status |= MIXER_BIAS_DETUNED; // set DETUNED bit

//   for ( i  = MIXER_BIAS_MIN;
//         i  < MIXER_BIAS_MAX;
//         i += MIXER_BIAS_SWEEP_STEP )
//     DAC7718_SetOutput( DAC7718_BIAS_CHANNEL( selected_mixer ), (uint16_t) i );

//   for ( i  = MIXER_BIAS_MAX;
//         i >= MIXER_BIAS_MIN;
//         i -= MIXER_BIAS_SWEEP_STEP )
//     DAC7718_SetOutput( DAC7718_BIAS_CHANNEL( selected_mixer ), (uint16_t) i );
// }


// /*! sweep mixer magnet current up and down */
// void mixer_magnet_sweep_once( void )
// {
//   int16_t i;

//   if ( NO_CHANNEL == selected_mixer )
//     return;

//   if ( mixer[ selected_mixer ].status & MIXER_DISABLED )
//     return;  // sweep only, if mixer is enabled

//   mixer[ selected_mixer ].status |= MIXER_MAGNET_DETUNED; // set DETUNED bit

//   for ( i  = MIXER_MAGNET_MIN;
//         i  < MIXER_MAGNET_MAX;
//         i += MIXER_MAGNET_SWEEP_STEP )
//     DAC7718_SetOutput( DAC7718_MAGNET_CHANNEL( selected_mixer ),
//                        (uint16_t) i );

//   for ( i  = MIXER_MAGNET_MAX;
//         i >= MIXER_MAGNET_MIN;
//         i -= MIXER_MAGNET_SWEEP_STEP )
//     DAC7718_SetOutput( DAC7718_MAGNET_CHANNEL( selected_mixer ),
//                        (uint16_t) i );
// }



