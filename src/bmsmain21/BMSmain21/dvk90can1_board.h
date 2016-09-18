//******************************************************************************
//! @file $RCSfile: dvk90can1_board.h,v $
//!
//! Copyright (c) 2007 Atmel.
//!
//! Use of this program is subject to Atmel's End User License Agreement.
//! Please read file license.txt for copyright notice.
//!
//! @brief Targetted board (dvk90can1) include file found in "config.h".
//!
//! This file mainly reflects the switch configuration of the board
//! - For LEDs: Define the full uC PORT linked to 8-bit of LEDS
//! - For Keyboard: Define PORT and bits for compass card KEYs.
//! - For ADC: Define PORT and channel configuration for sensors.
//! - For UART: Define which UART towards two is used.
//! - Hardware condition: For boot or application start.
//!
//! This file can be parsed by Doxygen for automatic documentation generation.
//! This file has been validated with AVRStudio-413528/WinAVR-20070122.
//!
//! @version $Revision: 3.20 $ $Name: jtellier $
//!
//! @todo
//! @bug
//******************************************************************************

#ifndef _BOARD_H_
#define _BOARD_H_

//_____ I N C L U D E S ________________________________________________________

//_____ M A C R O S ____________________________________________________________

//_____ D E F I N I T I O N S __________________________________________________

//! Define PORT for LEDs in DVK90CAN1.
#define LED_PORT_IN     PINA
#define LED_PORT_DIR    DDRA
#define LED_PORT_OUT   PORTA

//! Define PORT and bits for KEYs in DVK90CAN1.
//!
//! Case1:
//!    Bit   |  7 6 5 4  3 2 1 0
//!    ------+-------------------
//!    PORTE |  S W E N    C
//!    ------+-------------------
//!
//! Case2:
//!    Bit   |  7 6 5 4  3 2 1 0        Bit   |  7 6 5 4  3 2 1 0
//!    ------+-------------------  and  ------+-------------------
//!    PORTE |  S W E N                 PORTD |               C
//!    ------+-------------------       ------+-------------------
#define KEY_PORT_IN      PINE
#define KEY_PORT_DIR     DDRE
#define KEY_PORT_OUT    PORTE
#define KEY_NORTH           4
#define KEY_SOUTH           7
#define KEY_WEST            6
#define KEY_EAST            5
#define KEY_CENTER          2    // Configure the board with the appropriated
                                 // setting (J29)
// If case2, enable the following definitions
//#define KEY_CENTER_PORT_IN  PIND
//#define KEY_CENTER_PORT_DIR DDRD
//#define KEY_CENTER_PORT_OUT PORTD
//#define KEY_CENTER_IN   1       // PORTD.1

//! Define PORT for ADC.
#define ADC_PORT_IN        PINF
#define ADC_PORT_DIR       DDRF
#define ADC_PORT_OUT      PORTF
#define ADC_DID_REG       DIDR0
#define TEMPERATURE_CHANNEL   0  // MUX[4..0]=00000
#define LUMINOSITY_CHANNEL    1  // MUX[4..0]=00001
#define VIN_CHANNEL           2  // MUX[4..0]=00010
#define TEMPERATURE_INPUT_PIN 0
#define LUMINOSITY_INPUT_PIN  1
#define VIN_INPUT_PIN         2
#define AVCC_EQUAL_VCC           // Board configured with the appropriated setting (J19)
#define AREF_IS_OUTPUT_PIN       // Board configured with the appropriated setting (J15)

                                 //! Define UART to use.
//#define USE_UART         0       // Only UART-0 will be used ! Board configured with the
                                 // appropriated setting (J5)

//! Define PORT for audio.
#define AUDIO_PORT_IN        PINB
#define AUDIO_PORT_DIR       DDRB
#define AUDIO_PORT_OUT      PORTB
#define AUDIO_OUTPUT_PIN        7

//! Define for RTC.
#define RTC_TIMER               2  // RTC Timer number
#define RTC_CLOCK              32  // RTC frequency in KHz
//#define RTC_CLOCK               0  // SYSTEM clock (FOSC defined in "config.h") is used as RTC clock

//! Hardware condition (for boot or application start)
//!--- HWCB = INT on DVK90CAN1 board = INT0 or PD.0 - active low with pullup
#define HWCB_PORT_IN        PIND
#define HWCB_PORT_DIR       DDRD
#define HWCB_PORT_OUT      PORTD
#define HWCB_INPUT_PIN         0
#define HWCB_ACTIVE_LEVEL      0  // Active low = "0"
#define HWCB_PULLUP_ON         1  // With pullup ="1"
//!--- HWCB = Center Key on DVK90CAN1 board = PE.2 active low with pullup
//#define HWCB_PORT_IN        PINE
//#define HWCB_PORT_DIR       DDRE
//#define HWCB_PORT_OUT      PORTE
//#define HWCB_INPUT_PIN         2
//#define HWCB_ACTIVE_LEVEL      0  // Active low = "0"
//#define HWCB_PULLUP_ON         1  // With pullup ="1"

//_____ D E C L A R A T I O N S ________________________________________________

//_____ P R O T O T Y P E S ____________________________________________________

//______________________________________________________________________________

#endif  // _BOARD_H_



