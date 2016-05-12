//******************************************************************************
//! @file $RCSfile: config.h,v $
//!
//! Copyright (c) 2007 Atmel.
//!
//! Use of this program is subject to Atmel's End User License Agreement.
//! Please read file license.txt for copyright notice.
//!
//! @brief Configuration file for the following project:
//!             - can_spy_echo_example_gcc
//!
//! This file can be parsed by Doxygen for automatic documentation generation.
//! This file has been validated with AVRStudio-413528/WinAVR-20070122.
//!
//! @version $Revision: 3.20 $ $Name: jtellier $
//!
//! @todo
//! @bug
//******************************************************************************

#ifndef _CONFIG_H_
#define _CONFIG_H_

//_____ I N C L U D E S ________________________________________________________
#include "compiler.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "at90can_drv.h"
#include "dvk90can1_board.h"

//_____ M A C R O S ____________________________________________________________

//_____ D E F I N I T I O N S _______________________________________________

    // -------------- MCU LIB CONFIGURATION
#define FOSC           8000        // 8 MHz External cristal
#define F_CPU          (FOSC*1000) // Need for AVR GCC

    // -------------- UART LIB CONFIGURATION -1:
    // UART-0 or 1 if Baudrate changes during application
#define USE_TIMER16   BOTH_TIMER16
#define USE_UART      BOTH_UART
#define UART_BAUDRATE VARIABLE_UART_BAUDRATE
    // -------------- UART LIB CONFIGURATION -2:
    // UART-0 or 1 with fixed Baudrate all application long
//#define USE_TIMER16   TIMER16_1
//#define USE_UART      BOTH_UART
//#define UART_BAUDRATE 9600
    // -------------- UART LIB CONFIGURATION -3:
    // UART-0 or 1 with AutoBaudrate detection
//#define USE_TIMER16   BOTH_TIMER16
//#define USE_UART      BOTH_UART
//#define UART_BAUDRATE UART_AUTOBAUD

    // -------------- CAN LIB CONFIGURATION
#define CAN_BAUDRATE   125        // in kBit
//#define CAN_BAUDRATE   CAN_AUTOBAUD

    // -------------- MISCELLANEOUS
    // Using TIMER_2 as RTC
#define USE_TIMER8       TIMER8_2
//#define RTC_TIMER       2     // See "board.h"
//#define RTC_CLOCK       0     // See "board.h"

//_____ D E C L A R A T I O N S ________________________________________________

//______________________________________________________________________________

#endif  // _CONFIG_H_


