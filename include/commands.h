/*! \file commands.h
    \addtogroup SMART_bias_control

    List of commands to communicate with SMART bias electronics frontend

    Command structure:
    <address><code><parameters>

    <address>:   binary list of desired channels, e.g. 0x0C for channels 2,3
    <code>:      command byte (see below)
    <parameter>: required parameter(s) if any

    \author Urs U. Graf (graf@ph1.uni-koeln.de)

    \date started in 2014

    $Id:$
*/


/* Bias Settings */
#define SET_BIAS_VOLTAGE   0x10
#define GET_BIAS_SETTINGS  0x11

/* Magnet Current */
#define SET_MAGNET_CURRENT 0x20
#define GET_MAGNET_CURRENT 0x21

/* LNA bias */
#define SET_LNA_VOLTAGE_A  0x30
#define GET_LNA_VOLTAGE_A  0x31
#define SET_LNA_VOLTAGE_B  0x32
#define GET_LNA_VOLTAGE_B  0x33

/* Mixer status */
#define MIXER_STATUS       0x40
#define ENABLE_MIXER       0x41
#define DISABLE_MIXER      0x42
// #define SELECT_MIXER       0x43
// #define UNSELECT_MIXER     0x44
// #define LIST_ACTIVE_MIXERS 0x4F

/* Sweeps */
#define STOP_SWEEP         0x50
#define START_BIAS_SWEEP   0x51
#define START_MAGNET_SWEEP 0x52
#define BIAS_SCAN          0x53
#define MAGNET_SCAN        0x54

/* SRAM */
// #define DUMP_SRAM          0x60
// #define TEST_SRAM          0x61
// #define WRITE_SRAM_CELL    0x62
// #define READ_SRAM_CELL     0x63
// #define ERASE_SRAM         0x6F

/* Service */
// #define SET_DATA_PORT      0xF0
// #define SET_DEBUG_PORT     0xF1
// #define DAC7568_REF_ON     0xF2
// #define DAC7568_REF_OFF    0xF3
#define TEST               0xFA
#define TOGGLE_DEBUG       0xFD
#define STATUS             0xFE
#define RESET              0xFF

