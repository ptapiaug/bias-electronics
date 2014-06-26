/*! \file mixer.h
    \addtogroup SMART_bias_control

    Header file for high level mixer functions

    \author Urs U. Graf (graf@ph1.uni-koeln.de)

    \date started in 2014

    $Id:$
*/

#include "DAC7615.h"

// mixer structure
typedef struct mixer
{
  uint8_t status;
  int16_t bias_value;
  int16_t magnet_value;
} MIXER;


#define MIXER_DISABLED          0x1
#define MIXER_BIAS_DETUNED      0x2
#define MIXER_MAGNET_DETUNED    0x4
#define MIXER_NO_SWEEP          0
#define MIXER_BIAS_SWEEP        1
#define MIXER_MAGNET_SWEEP      2

// bias DAC range and sweep step size
#define MIXER_BIAS_MIN           0
#define MIXER_BIAS_MAX           4095
#define MIXER_BIAS_ZERO          2047
#define MIXER_BIAS_SWEEP_STEP    2
// magnet DAC range and sweep step size
#define MIXER_MAGNET_MIN         0
#define MIXER_MAGNET_MAX         4095
#define MIXER_MAGNET_ZERO        2047
#define MIXER_MAGNET_SWEEP_STEP  2


// prototypes

/*! initialize mixers */
void init_mixers( void );

/*! enable mixer */
void mixer_enable( void );

/*! disable mixer */
void mixer_disable( void);

/*! set mixer bias voltage */
void mixer_bias( int16_t v );

/*! restore bias voltage values (e.g. after sweep) */
void mixer_bias_restore( void );

/*! set mixer magnetic field */
void mixer_magnet( int16_t v );

/*! restore magnetic field values (e.g. after sweep) */
void mixer_magnet_restore( void );

/*! sweep mixer magnet current up and down */
void mixer_magnet_sweep_once( void );

/*! sweep mixer bias up and down */
void mixer_bias_sweep_once( void );

/*! bias Mixer Scan */
void mixer_bias_scan(void);