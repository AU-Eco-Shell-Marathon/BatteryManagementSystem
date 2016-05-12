//******************************************************************************
//! @file $RCSfile: timer16_drv.c,v $
//!
//! Copyright (c) 2007 Atmel.
//!
//! Use of this program is subject to Atmel's End User License Agreement.
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains the low level functions (drivers) of:
//!             - 16-bit Timer(s)
//!             - for AT90CAN128/64/32
//!
//! This file can be parsed by Doxygen for automatic documentation generation.
//! This file has been validated with AVRStudio-413528/WinAVR-20070122.
//!
//! @version $Revision: 3.20 $ $Name: jtellier $
//!
//! @todo
//! @bug
//******************************************************************************

//_____ I N C L U D E S ________________________________________________________
#include "config.h"
#include "timer16_drv.h"

//_____ D E F I N I T I O N S __________________________________________________

//_____ D E C L A R A T I O N S ________________________________________________

//_____ F U N C T I O N S ______________________________________________________

//------------------------------------------------------------------------------
//  @fn timer16_get_counter
//!
//! This function READ the 16-bit TIMER counter.
//!
//! @warning 
//!
//! @param  
//!
//! @return 16-bit counter value
//------------------------------------------------------------------------------
U16 timer16_get_counter(void)
{
    U16 u16_temp;
    
    u16_temp  =  Timer16_get_counter_low();
    u16_temp |= (Timer16_get_counter_high() << 8 );
    
    return u16_temp;
}
    
//------------------------------------------------------------------------------
//  @fn timer16_get_capture
//!
//! This function READ the 16-bit TIMER capture register.
//!
//! @warning 
//!
//! @param  
//!
//! @return 16-bit capture value
//------------------------------------------------------------------------------
U16 timer16_get_capture(void)
{
    U16 u16_temp;
    
    u16_temp  =  Timer16_get_capture_low();
    u16_temp |= (Timer16_get_capture_high() << 8 );
    
    return u16_temp;
}
