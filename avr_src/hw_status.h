/*! \file hw_status.h
    \addtogroup SMART_bias_control

    Header file to read hardware ID and status of uP

    \author Urs U. Graf (graf@ph1.uni-koeln.de)

    \date started in 2014

    $Id:$
*/

#ifndef _HW_STATUS_H
#define _HW_STATUS_H

/* read device ID registers */
char *read_device_id( void );

/*! read calibration information from NVM */
uint8_t ReadCalByte( uint8_t index );

/*! setup ADC A to read internal temperature sensor */
void temp_sensor_init( void );

/* read internal temperature sensor */
char *temp_sensor_readout( void );

#endif /* _HW_STATUS_H */