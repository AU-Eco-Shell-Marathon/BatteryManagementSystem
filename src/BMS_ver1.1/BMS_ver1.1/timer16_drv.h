///******************************************************************************
//! @file $RCSfile: timer16_drv.h,v $
//!
//! Copyright (c) 2007 Atmel.
//!
//! Use of this program is subject to Atmel's End User License Agreement.
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains the prototypes and the macros of the
//!        low level functions (drivers) of:
//!             - 16-bit timer(s)
//!             - for AT90CAN128/64/32.
//!
//! This file can be parsed by Doxygen for automatic documentation generation.
//! This file has been validated with AVRStudio-413528/WinAVR-20070122.
//!
//! @version $Revision: 3.20 $ $Name: jtellier $
//!
//! @todo
//! @bug
//******************************************************************************

#ifndef _TIMER16_DRV_H_
#define _TIMER16_DRV_H_

//_____ I N C L U D E S ________________________________________________________

#include "config.h"

//_____ G E N E R A L    D E F I N I T I O N S _________________________________

#ifndef FOSC
#  error  You must define FOSC in "config.h" file
#endif
    // ----------
#ifndef TIMER16_1            // 16-bit TIMER 1 Defintion
#define TIMER16_1       0x01
#endif
#ifndef TIMER16_3            // 16-bit TIMER 3 Defintion
#define TIMER16_3       0x03
#endif
#ifndef BOTH_TIMER16         // Both the 16-bit TIMERs Defintion
#define BOTH_TIMER16    0xFF
#endif

//_____ M A C R O S ____________________________________________________________

    // ---------- To order the loading (reading) of 16-bit registers
#define Timer16_get_counter()       ( timer16_get_counter() )        // c.f. "timer16_drv.c" file
#define Timer16_get_capture()       ( timer16_get_capture() )        // c.f. "timer16_drv.c" file
    // ---------- Two ways to have a look on the things
#define Timer16_set_pwm_a(value)    ( Timer16_set_compare_a(value) ) // c.f. above !
#define Timer16_set_pwm_b(value)    ( Timer16_set_compare_b(value) ) // c.f. above !
#define Timer16_set_pwm_c(value)    ( Timer16_set_compare_c(value) ) // c.f. above !
#define Timer16_get_pwm_a()         ( Timer16_get_compare_a() )      // c.f. above !
#define Timer16_get_pwm_b()         ( Timer16_get_compare_b() )      // c.f. above !
#define Timer16_get_pwm_c()         ( Timer16_get_compare_c() )      // c.f. above !
    // ---------- If no clock, the timer is off !
#define Timer16_off()                 Timer16_set_clock(TIMER16_NO_CLOCK)

//_____ D E F .  &   M A C R O S   for   H W   C O N F . _______________________

    //----- CARE WITH THE ORDER WHEN 16-BIT REGISTERS ARE READ
    //      ==================================================
    //----- For sensitive 16-bit registers (c.f. temporary reg), the macros are:
    //-----     *  Timer16_get_nnn_low()
    //-----     *  Timer16_get_nnn_high()
    //----- For instance, in your main, do not write:
    //-----     short_temp = ((Timer16_get_nnn_high())<<8) | (Timer16_get_nnn_low());
    //-----   or
    //-----     short_temp = (Timer16_get_nnn_low()) | ((Timer16_get_nnn_high())<<8);
    //-----   because IAR and ImageCraft doesn't evaluate the operandes in the same order!
    //-----
    //----- The good way to write a READ (load) sequence is in 2 times:
    //-----     short_temp  =  Timer16_get_nnn_low();
    //-----     short_temp |= (Timer16_get_nnn_high() << 8 );
    //-----
    //----- Otherwise a macro "Timer16_get_nnn()" exits and call "timer16_get_counter()" function

#ifndef USE_TIMER16
#       error You must define USE_TIMER16 to TIMER16_1 or TIMER16_3 or BOTH_TIMER16 in "config.h" file
#   elif (USE_TIMER16 == TIMER16_1)        //!< 16-bit TIMER 1 used
    //!< =================================
    //!< ------ Only TIMER16_1 used ------
    //!< =================================
#       define Timer16_select(timer16_num)     // Empty !
    // ---------- Macros
#       define Timer16_clear()  ( TCCR1B=0, TCCR1A=0, TCCR1C=0, TCNT1H=0, TCNT1L= 0, OCR1AH=0, OCR1AL=0, \
                                  OCR1BH=0, OCR1BL=0, OCR1CH=0, OCR1CL=0, ICR1H=0, ICR1L=0              )
        // ----------
#       define Timer16_set_counter(value)       ( TCNT1H = ((U8)(value>>8)), TCNT1L = ((U8)(value)))
#       define Timer16_get_counter_low()        ((U16)(TCNT1L))
#       define Timer16_get_counter_high()       ((U16)(TCNT1H))
        // ----------
#       define Timer16_set_compare_a(value)     ( OCR1AH = ((U8)(value>>8)), OCR1AL = ((U8)(value)))
#       define Timer16_set_compare_b(value)     ( OCR1BH = ((U8)(value>>8)), OCR1BL = ((U8)(value)))
#       define Timer16_set_compare_c(value)     ( OCR1CH = ((U8)(value>>8)), OCR1CL = ((U8)(value)))
#       define Timer16_get_compare_a()          ( OCR1A )      // The temporary register is not used
#       define Timer16_get_compare_b()          ( OCR1B )      // The temporary register is not used
#       define Timer16_get_compare_c()          ( OCR1C )      // The temporary register is not used
        // ----------
#       define Timer16_set_capture(value)       { ICR1H = ((U8)(value>>8)); ICR1L = ((U8)(value)); }
#       define Timer16_get_capture_low()        ((U16)(ICR1L))
#       define Timer16_get_capture_high()       ((U16)(ICR1H))
        // ----------
#       define Timer16_set_mode_output_a(conf)  ( TCCR1A = (TCCR1A & (~TIMER16_COMP_MODE_MASK_A)) | (conf << COM1A0) )
#       define Timer16_set_mode_output_b(conf)  ( TCCR1A = (TCCR1A & (~TIMER16_COMP_MODE_MASK_B)) | (conf << COM1B0) )
#       define Timer16_set_mode_output_c(conf)  ( TCCR1A = (TCCR1A & (~TIMER16_COMP_MODE_MASK_C)) | (conf << COM1C0) )
#       define Timer16_get_mode_output_a()      ((TCCR1A & TIMER16_COMP_MODE_MASK_A) >> COM1A0 )
#       define Timer16_get_mode_output_b()      ((TCCR1A & TIMER16_COMP_MODE_MASK_B) >> COM1B0 )
#       define Timer16_get_mode_output_c()      ((TCCR1A & TIMER16_COMP_MODE_MASK_C) >> COM1C0 )
        // ----------
#       define Timer16_set_waveform_mode(conf)  { TCCR1A = (TCCR1A & (~TIMER16_WGM_RA_MASK)) | ((conf &  0x3) << WGM10); \
                                                  TCCR1B = (TCCR1B & (~TIMER16_WGM_RB_MASK)) | ((conf >> 0x2) << WGM12)  }
#       define Timer16_get_waveform_mode()     (((TCCR1A & TIMER16_WGM_RA_MASK) >> WGM10) |         \
                                               (((TCCR1B & TIMER16_WGM_RB_MASK) >> WGM12) << 0x2)   )
        // ----------
#       define Timer16_set_clock(value)        ( TCCR1B = (TCCR1B & (~TIMER16_CLK_MASK)) | (value << CS10) )
#       define Timer16_get_clock()            (((TCCR1B & TIMER16_CLK_MASK) >> CS10) )
        // ----------
#       define Timer16_set_input_filter()      ( TCCR1B |=  (1<<ICNC1) )
#       define Timer16_clear_input_filter()    ( TCCR1B &= ~(1<<ICNC1) )
#       define Timer16_get_input_filter()      ((TCCR1B  &  (1<<ICNC1)) >> ICNC1 )
    // ----------
#       define Timer16_set_input_rising_edge() ( TCCR1B |=  (1<<ICES1) )
#       define Timer16_set_input_falling_edge()( TCCR1B &= ~(1<<ICES1) )
#       define Timer16_get_input_capture_edge()((TCCR1B &   (1<<ICES1)) >> ICES1 )
    // ----------
#       define Timer16_set_compare_force_a()   ( TCCR1C |=  (1<<FOC1A) )
#       define Timer16_set_compare_force_b()   ( TCCR1C |=  (1<<FOC1B) )
#       define Timer16_set_compare_force_c()   ( TCCR1C |=  (1<<FOC1C) )
#       define Timer16_clear_compare_force_a() ( TCCR1C &= ~(1<<FOC1A) )
#       define Timer16_clear_compare_force_b() ( TCCR1C &= ~(1<<FOC1B) )
#       define Timer16_clear_compare_force_c() ( TCCR1C &= ~(1<<FOC1C) )
#       define Timer16_get_compare_force_a()   ((TCCR1C  &  (1<<FOC1A)) >> FOC1A )
#       define Timer16_get_compare_force_b()   ((TCCR1C  &  (1<<FOC1B)) >> FOC1B )
#       define Timer16_get_compare_force_c()   ((TCCR1C  &  (1<<FOC1C)) >> FOC1C )
    // ----------
#       define Timer16_overflow_it_enable()    ( TIMSK1 |=  (1<<TOIE1)  )
#       define Timer16_overflow_it_disable()   ( TIMSK1 &= ~(1<<TOIE1)  )
#       define Timer16_compare_a_it_enable()   ( TIMSK1 |=  (1<<OCIE1A) )
#       define Timer16_compare_a_it_disable()  ( TIMSK1 &= ~(1<<OCIE1A) )
#       define Timer16_compare_b_it_enable()   ( TIMSK1 |=  (1<<OCIE1B) )
#       define Timer16_compare_b_it_disable()  ( TIMSK1 &= ~(1<<OCIE1B) )
#       define Timer16_compare_c_it_enable()   ( TIMSK1 |=  (1<<OCIE1C) )
#       define Timer16_compare_c_it_disable()  ( TIMSK1 &= ~(1<<OCIE1C) )
#       define Timer16_capture_it_enable()     ( TIMSK1 |=  (1<<ICIE1)  )
#       define Timer16_capture_it_disable()    ( TIMSK1 &= ~(1<<ICIE1)  )
#       define Timer16_get_overflow_it_mask()  ((TIMSK1  &  (1<<TOIE1) ) >> TOIE1  )
#       define Timer16_get_compare_a_it_mask() ((TIMSK1  &  (1<<OCIE1A)) >> OCIE1A )
#       define Timer16_get_compare_b_it_mask() ((TIMSK1  &  (1<<OCIE1B)) >> OCIE1B )
#       define Timer16_get_compare_c_it_mask() ((TIMSK1  &  (1<<OCIE1C)) >> OCIE1C )
#       define Timer16_get_capture_it_mask()   ((TIMSK1  &  (1<<ICIE1) ) >> ICIE1  )
    // ----------
#       define Timer16_clear_overflow_it()     ( TIFR1 |=  (1<<TOV1)  )
#       define Timer16_clear_compare_a_it()    ( TIFR1 |=  (1<<OCF1A) )
#       define Timer16_clear_compare_b_it()    ( TIFR1 |=  (1<<OCF1B) )
#       define Timer16_clear_compare_c_it()    ( TIFR1 |=  (1<<OCF1C) )
#       define Timer16_clear_capture_it()      ( TIFR1 |=  (1<<ICF1)  )
#       define Timer16_get_overflow_it()       ((TIFR1  &  (1<<TOV1) ) >> TOV1  )
#       define Timer16_get_compare_a_it()      ((TIFR1  &  (1<<OCF1A)) >> OCF1A )
#       define Timer16_get_compare_b_it()      ((TIFR1  &  (1<<OCF1B)) >> OCF1B )
#       define Timer16_get_compare_c_it()      ((TIFR1  &  (1<<OCF1C)) >> OCF1C )
#       define Timer16_get_capture_it()        ((TIFR1  &  (1<<ICF1) ) >> ICF1  )

#   elif USE_TIMER16 == TIMER16_3          //!< 16-bit TIMER 3 used
    //!< =================================
    //!< ------ Only TIMER16_3 used ------
    //!< =================================
#       define Timer16_select(timer16_num)     // Empty !
    // ---------- Macros
#       define Timer16_clear()  { TCCR3B=0; TCCR3A=0; TCCR3C=0; TCNT3H=0; TCNT3L= 0; OCR3AH=0; OCR3AL=0; \
                                  OCR3BH=0; OCR3BL=0; OCR3CH=0; OCR3CL=0; ICR3H=0, ICR3L=0;              }
        // ----------
#       define Timer16_set_counter(value)       ( TCNT3H = ((U8)(value>>8)), TCNT3L = ((U8)(value)))
#       define Timer16_get_counter_low()        ((U16)(TCNT3L))
#       define Timer16_get_counter_high()       ((U16)(TCNT3H))
        // ----------
#       define Timer16_set_compare_a(value)     { OCR3AH = ((U8)(value>>8)); OCR3AL = ((U8)(value)); }
#       define Timer16_set_compare_b(value)     { OCR3BH = ((U8)(value>>8)); OCR3BL = ((U8)(value)); }
#       define Timer16_set_compare_c(value)     { OCR3CH = ((U8)(value>>8)); OCR3CL = ((U8)(value)); }
#       define Timer16_get_compare_a()          ( OCR3A )      // The temporary register is not used
#       define Timer16_get_compare_b()          ( OCR3B )      // The temporary register is not used
#       define Timer16_get_compare_c()          ( OCR3C )      // The temporary register is not used
        // ----------
#       define Timer16_set_capture(value)       { ICR3H = ((U8)(value>>8)); ICR3L = ((U8)(value)); }
#       define Timer16_get_capture_low()        ((U16)(ICR3L))
#       define Timer16_get_capture_high()       ((U16)(ICR3H))
        // ----------
#       define Timer16_set_mode_output_a(conf)  ( TCCR3A = (TCCR3A & (~TIMER16_COMP_MODE_MASK_A)) | (conf << COM3A0) )
#       define Timer16_set_mode_output_b(conf)  ( TCCR3A = (TCCR3A & (~TIMER16_COMP_MODE_MASK_B)) | (conf << COM3B0) )
#       define Timer16_set_mode_output_c(conf)  ( TCCR3A = (TCCR3A & (~TIMER16_COMP_MODE_MASK_C)) | (conf << COM3C0) )
#       define Timer16_get_mode_output_a()      ((TCCR3A & TIMER16_COMP_MODE_MASK_A) >> COM3A0 )
#       define Timer16_get_mode_output_b()      ((TCCR3A & TIMER16_COMP_MODE_MASK_B) >> COM3B0 )
#       define Timer16_get_mode_output_c()      ((TCCR3A & TIMER16_COMP_MODE_MASK_C) >> COM3C0 )
        // ----------
#       define Timer16_set_waveform_mode(conf)  ( TCCR3A = (TCCR3A & (~TIMER16_WGM_RA_MASK)) | ((conf &  0x3) << WGM30), \
                                                  TCCR3B = (TCCR3B & (~TIMER16_WGM_RB_MASK)) | ((conf >> 0x2) << WGM32)  )
#       define Timer16_get_waveform_mode()     (((TCCR3A & TIMER16_WGM_RA_MASK) >> WGM30) |         \
                                               (((TCCR3B & TIMER16_WGM_RB_MASK) >> WGM32) << 0x2)   )
        // ----------
#       define Timer16_set_clock(value)        ( TCCR3B = (TCCR3B & (~TIMER16_CLK_MASK)) | (value << CS30) )
#       define Timer16_get_clock()            (((TCCR3B & TIMER16_CLK_MASK) >> CS30) )
        // ----------
#       define Timer16_set_input_filter()      ( TCCR3B |=  (1<<ICNC3) )
#       define Timer16_clear_input_filter()    ( TCCR3B &= ~(1<<ICNC3) )
#       define Timer16_get_input_filter()      ((TCCR3B  &  (1<<ICNC3)) >> ICNC3 )
    // ----------
#       define Timer16_set_input_rising_edge() ( TCCR3B |=  (1<<ICES3) )
#       define Timer16_set_input_falling_edge()( TCCR3B &= ~(1<<ICES3) )
#       define Timer16_get_input_capture_edge()((TCCR3B &   (1<<ICES3)) >> ICES3 )
    // ----------
#       define Timer16_set_compare_force_a()   ( TCCR3C |=  (1<<FOC3A) )
#       define Timer16_set_compare_force_b()   ( TCCR3C |=  (1<<FOC3B) )
#       define Timer16_set_compare_force_c()   ( TCCR3C |=  (1<<FOC3C) )
#       define Timer16_clear_compare_force_a() ( TCCR3C &= ~(1<<FOC3A) )
#       define Timer16_clear_compare_force_b() ( TCCR3C &= ~(1<<FOC3B) )
#       define Timer16_clear_compare_force_c() ( TCCR3C &= ~(1<<FOC3C) )
#       define Timer16_get_compare_force_a()   ((TCCR3C  &  (1<<FOC3A)) >> FOC3A )
#       define Timer16_get_compare_force_b()   ((TCCR3C  &  (1<<FOC3B)) >> FOC3B )
#       define Timer16_get_compare_force_c()   ((TCCR3C  &  (1<<FOC3C)) >> FOC3C )
    // ----------
#       define Timer16_overflow_it_enable()    ( TIMSK3 |=  (1<<TOIE3)  )
#       define Timer16_overflow_it_disable()   ( TIMSK3 &= ~(1<<TOIE3)  )
#       define Timer16_compare_a_it_enable()   ( TIMSK3 |=  (1<<OCIE3A) )
#       define Timer16_compare_a_it_disable()  ( TIMSK3 &= ~(1<<OCIE3A) )
#       define Timer16_compare_b_it_enable()   ( TIMSK3 |=  (1<<OCIE3B) )
#       define Timer16_compare_b_it_disable()  ( TIMSK3 &= ~(1<<OCIE3B) )
#       define Timer16_compare_c_it_enable()   ( TIMSK3 |=  (1<<OCIE3C) )
#       define Timer16_compare_c_it_disable()  ( TIMSK3 &= ~(1<<OCIE3C) )
#       define Timer16_capture_it_enable()     ( TIMSK3 |=  (1<<ICIE3)  )
#       define Timer16_capture_it_disable()    ( TIMSK3 &= ~(1<<ICIE3)  )
#       define Timer16_get_overflow_it_mask()  ((TIMSK3  &  (1<<TOIE3) ) >> TOIE3  )
#       define Timer16_get_compare_a_it_mask() ((TIMSK3  &  (1<<OCIE3A)) >> OCIE3A )
#       define Timer16_get_compare_b_it_mask() ((TIMSK3  &  (1<<OCIE3B)) >> OCIE3B )
#       define Timer16_get_compare_c_it_mask() ((TIMSK3  &  (1<<OCIE3C)) >> OCIE3C )
#       define Timer16_get_capture_it_mask()   ((TIMSK3  &  (1<<ICIE3) ) >> ICIE3  )
    // ----------
#       define Timer16_clear_overflow_it()     ( TIFR3 |=  (1<<TOV3)  )
#       define Timer16_clear_compare_a_it()    ( TIFR3 |=  (1<<OCF3A) )
#       define Timer16_clear_compare_b_it()    ( TIFR3 |=  (1<<OCF3B) )
#       define Timer16_clear_compare_c_it()    ( TIFR3 |=  (1<<OCF3C) )
#       define Timer16_clear_capture_it()      ( TIFR3 |=  (1<<ICF3)  )
#       define Timer16_get_overflow_it()       ((TIFR3  &  (1<<TOV3) ) >> TOV3  )
#       define Timer16_get_compare_a_it()      ((TIFR3  &  (1<<OCF3A)) >> OCF3A )
#       define Timer16_get_compare_b_it()      ((TIFR3  &  (1<<OCF3B)) >> OCF3B )
#       define Timer16_get_compare_c_it()      ((TIFR3  &  (1<<OCF3C)) >> OCF3C )
#       define Timer16_get_capture_it()        ((TIFR3  &  (1<<ICF3) ) >> ICF3  )

#   elif USE_TIMER16 == BOTH_TIMER16   //!< Both the 16-bit TIMERs
    //!< =========================================
    //!< ------ Both TIMER16_1 & 3 are used ------
    //!< =========================================
        extern U8 timer16_selected;         // $$$-- EXTERNAL DECLARATION --$$$
#       define Timer16_select(timer16_num)     (timer16_selected=timer16_num)
    // ---------- Macros
#       define Timer16_clear()              ((timer16_selected==TIMER16_1)? \
                                                (TCCR1B=0, TCCR1A=0, TCCR1C=0, TCNT1H=0, TCNT1L= 0, OCR1AH=0, OCR1AL=0, \
                                                 OCR1BH=0, OCR1BL=0, OCR1CH=0, OCR1CL=0, ICR1H=0, ICR1L=0 )             \
                                                : \
                                                (TCCR3B=0, TCCR3A=0, TCCR3C=0, TCNT3H=0, TCNT3L= 0, OCR3AH=0, OCR3AL=0, \
                                                 OCR3BH=0, OCR3BL=0, OCR3CH=0, OCR3CL=0, ICR3H=0, ICR3L=0 )             )
        // ----------
#       define Timer16_set_counter(value)   ((timer16_selected==TIMER16_1)? \
                                                (TCNT1H = ((U8)(value>>8)), TCNT1L = ((U8)(value)) ) \
                                                : \
                                                (TCNT3H = ((U8)(value>>8)), TCNT3L = ((U8)(value)) ) )
#       define Timer16_get_counter_low()    ((timer16_selected==TIMER16_1)? ((U16)(TCNT1L)) : ((U16)(TCNT3L)) )
#       define Timer16_get_counter_high()   ((timer16_selected==TIMER16_1)? ((U16)(TCNT1H)) : ((U16)(TCNT3H)) )
        // ----------
#       define Timer16_set_compare_a(value) ((timer16_selected==TIMER16_1)? \
                                                (OCR1AH = ((U8)(value>>8)), OCR1AL = ((U8)(value)) ) \
                                                : \
                                                (OCR3AH = ((U8)(value>>8)), OCR3AL = ((U8)(value)) ) )
#       define Timer16_set_compare_b(value) ((timer16_selected==TIMER16_1)? \
                                                (OCR1BH = ((U8)(value>>8)), OCR1BL = ((U8)(value)) ) \
                                                : \
                                                (OCR3BH = ((U8)(value>>8)), OCR3BL = ((U8)(value)) ) )
#       define Timer16_set_compare_c(value) ((timer16_selected==TIMER16_1)? \
                                                (OCR3CH = ((U8)(value>>8)), OCR3CL = ((U8)(value)) ) \
                                                : \
                                                (OCR1CH = ((U8)(value>>8)), OCR1CL = ((U8)(value)) ) )
#       define Timer16_get_compare_a()      ((timer16_selected==TIMER16_1)? (OCR1A) : (OCR3A) )   // The temporary register is not used
#       define Timer16_get_compare_b()      ((timer16_selected==TIMER16_1)? (OCR1B) : (OCR3B) )   // The temporary register is not used
#       define Timer16_get_compare_c()      ((timer16_selected==TIMER16_1)? (OCR1C) : (OCR3C) )   // The temporary register is not used
        // ----------
#       define Timer16_set_capture(value)   ((timer16_selected==TIMER16_1)? \
                                                (ICR1H = ((U8)(value>>8)), ICR1L = ((U8)(value))) \
                                                : \
                                                (ICR3H = ((U8)(value>>8)), ICR3L = ((U8)(value))) )
#       define Timer16_get_capture_low()    ((timer16_selected==TIMER16_1)? ((U16)(ICR1L)) : ((U16)(ICR3L)) )
#       define Timer16_get_capture_high()   ((timer16_selected==TIMER16_1)? ((U16)(ICR1H)) : ((U16)(ICR3H)) )
        // ----------
#       define Timer16_set_mode_output_a(conf)  ((timer16_selected==TIMER16_1)? \
                                                    (TCCR1A = (TCCR1A & (~TIMER16_COMP_MODE_MASK_A)) | (conf << COM1A0)) \
                                                    : \
                                                    (TCCR3A = (TCCR3A & (~TIMER16_COMP_MODE_MASK_A)) | (conf << COM3A0)) )
#       define Timer16_set_mode_output_b(conf)  ((timer16_selected==TIMER16_1)? \
                                                    (TCCR1A = (TCCR1A & (~TIMER16_COMP_MODE_MASK_B)) | (conf << COM1B0)) \
                                                    : \
                                                    (TCCR3A = (TCCR3A & (~TIMER16_COMP_MODE_MASK_B)) | (conf << COM3B0)) )
#       define Timer16_set_mode_output_c(conf)  ((timer16_selected==TIMER16_1)? \
                                                    (TCCR1A = (TCCR1A & (~TIMER16_COMP_MODE_MASK_C)) | (conf << COM1C0)) \
                                                    : \
                                                    (TCCR3A = (TCCR3A & (~TIMER16_COMP_MODE_MASK_C)) | (conf << COM3C0)) )
#       define Timer16_get_mode_output_a()   ((timer16_selected==TIMER16_1)? \
                                                ((TCCR1A & TIMER16_COMP_MODE_MASK_A) >> COM1A0) : ((TCCR3A & TIMER16_COMP_MODE_MASK_A) >> COM3A0 ) )
#       define Timer16_get_mode_output_b()   ((timer16_selected==TIMER16_1)? \
                                                ((TCCR1A & TIMER16_COMP_MODE_MASK_B) >> COM1B0) : ((TCCR3A & TIMER16_COMP_MODE_MASK_B) >> COM3B0) )
#       define Timer16_get_mode_output_c()   ((timer16_selected==TIMER16_1)? \
                                                ((TCCR1A & TIMER16_COMP_MODE_MASK_C) >> COM1C0) : ((TCCR3A & TIMER16_COMP_MODE_MASK_C) >> COM3C0) )
        // ----------
#       define Timer16_set_waveform_mode(conf)  ((timer16_selected==TIMER16_1)? \
                                                    (TCCR1A = (TCCR1A & (~TIMER16_WGM_RA_MASK)) | ((conf &  0x3) << WGM10) , \
                                                     TCCR1B = (TCCR1B & (~TIMER16_WGM_RB_MASK)) | ((conf >> 0x2) << WGM12) ) \
                                                     : \
                                                    (TCCR3A = (TCCR3A & (~TIMER16_WGM_RA_MASK)) | ((conf &  0x3) << WGM30) , \
                                                     TCCR3B = (TCCR3B & (~TIMER16_WGM_RB_MASK)) | ((conf >> 0x2) << WGM32) ) )
#       define Timer16_get_waveform_mode()   ((timer16_selected==TIMER16_1)? \
                                                (((TCCR1A & TIMER16_WGM_RA_MASK) >> WGM10) | (((TCCR1B & TIMER16_WGM_RB_MASK) >> WGM12) << 0x2)) \
                                                : \
                                                (((TCCR3A & TIMER16_WGM_RA_MASK) >> WGM30) | (((TCCR3B & TIMER16_WGM_RB_MASK) >> WGM32) << 0x2)) )
        // ----------
#       define Timer16_set_clock(value)      ((timer16_selected==TIMER16_1)? \
                                                (TCCR1B = (TCCR1B & (~TIMER16_CLK_MASK)) | (value << CS10)) \
                                                : \
                                                (TCCR3B = (TCCR3B & (~TIMER16_CLK_MASK)) | (value << CS30)) )

#       define Timer16_get_clock()           ((timer16_selected==TIMER16_1)? \
                                                (((TCCR1B & TIMER16_CLK_MASK) >> CS10)) : (((TCCR3B & TIMER16_CLK_MASK) >> CS30)) )
        // ----------
#       define Timer16_set_input_filter()    ((timer16_selected==TIMER16_1)? \
                                                (TCCR1B |= (1<<ICNC1)) : ( TCCR3B |= (1<<ICNC3)) )
#       define Timer16_clear_input_filter()  ((timer16_selected==TIMER16_1)? \
                                                (TCCR1B &= ~(1<<ICNC1)) :  ( TCCR3B &= ~(1<<ICNC3)) )
#       define Timer16_get_input_filter()    ((timer16_selected==TIMER16_1)? \
                                                ((TCCR1B & (1<<ICNC1)) >> ICNC1) : ((TCCR3B & (1<<ICNC3)) >> ICNC3) )
    // ----------
#       define Timer16_set_input_rising_edge()  ((timer16_selected==TIMER16_1)? \
                                                    (TCCR1B |= (1<<ICES1)) : (TCCR3B |= (1<<ICES3)) )
#       define Timer16_set_input_falling_edge() ((timer16_selected==TIMER16_1)? \
                                                    (TCCR1B &= ~(1<<ICES1)) : ( TCCR3B &= ~(1<<ICES3)) )
#       define Timer16_get_input_capture_edge() ((timer16_selected==TIMER16_1)? \
                                                    ((TCCR1B & (1<<ICES1)) >> ICES1) : ((TCCR3B & (1<<ICES3)) >> ICES3) )
    // ----------
#       define Timer16_set_compare_force_a()    ((timer16_selected==TIMER16_1)? \
                                                    (TCCR1C |= (1<<FOC1A)) : (TCCR3C |= (1<<FOC3A)) )
#       define Timer16_set_compare_force_b()    ((timer16_selected==TIMER16_1)? \
                                                    (TCCR1C |= (1<<FOC1B)) : (TCCR3C |= (1<<FOC3B)) )
#       define Timer16_set_compare_force_c()    ((timer16_selected==TIMER16_1)? \
                                                    (TCCR1C |= (1<<FOC1C)) : (TCCR3C |= (1<<FOC3C)) )
#       define Timer16_clear_compare_force_a()  ((timer16_selected==TIMER16_1)? \
                                                    (TCCR1C &= ~(1<<FOC1A ) : (TCCR3C &= ~(1<<FOC3A)) )
#       define Timer16_clear_compare_force_b()  ((timer16_selected==TIMER16_1)? \
                                                    (TCCR1C &= ~(1<<FOC1B)) : (TCCR3C &= ~(1<<FOC3B)) )
#       define Timer16_clear_compare_force_c()  ((timer16_selected==TIMER16_1)? \
                                                    (TCCR1C &= ~(1<<FOC1C)) : (TCCR3C &= ~(1<<FOC3C)) )
#       define Timer16_get_compare_force_a()    ((timer16_selected==TIMER16_1)? \
                                                    ((TCCR1C & (1<<FOC1A)) >> FOC1A) : ((TCCR3C & (1<<FOC3A)) >> FOC3A) )
#       define Timer16_get_compare_force_b()    ((timer16_selected==TIMER16_1)? \
                                                    ((TCCR1C & (1<<FOC1B)) >> FOC1B) : ((TCCR3C & (1<<FOC3B)) >> FOC3B) )
#       define Timer16_get_compare_force_c()    ((timer16_selected==TIMER16_1)? \
                                                    ((TCCR1C & (1<<FOC1C)) >> FOC1C) : ((TCCR3C & (1<<FOC3C)) >> FOC3C) )
    // ----------
#       define Timer16_overflow_it_enable()     ((timer16_selected==TIMER16_1)? \
                                                    (TIMSK1 |= (1<<TOIE1)) : (TIMSK3 |= (1<<TOIE3)) )
#       define Timer16_overflow_it_disable()    ((timer16_selected==TIMER16_1)? \
                                                    (TIMSK1 &= ~(1<<TOIE1)) : (TIMSK3 &= ~(1<<TOIE3)) )
#       define Timer16_compare_a_it_enable()    ((timer16_selected==TIMER16_1)? \
                                                    (TIMSK1 |= (1<<OCIE1A)) : (TIMSK3 |= (1<<OCIE3A)) )
#       define Timer16_compare_a_it_disable()   ((timer16_selected==TIMER16_1)? \
                                                    (TIMSK1 &= ~(1<<OCIE1A)) : (TIMSK3 &= ~(1<<OCIE3A)) )
#       define Timer16_compare_b_it_enable()    ((timer16_selected==TIMER16_1)? \
                                                    (TIMSK1 |= (1<<OCIE1B)) : (TIMSK3 |= (1<<OCIE3B)) )
#       define Timer16_compare_b_it_disable()   ((timer16_selected==TIMER16_1)? \
                                                    (TIMSK1 &= ~(1<<OCIE1B)) : (TIMSK3 &= ~(1<<OCIE3B)) )
#       define Timer16_compare_c_it_enable()    ((timer16_selected==TIMER16_1)? \
                                                    (TIMSK1 |= (1<<OCIE1C)) : (TIMSK3 |= (1<<OCIE3C)) )
#       define Timer16_compare_c_it_disable()   ((timer16_selected==TIMER16_1)? \
                                                    (TIMSK1 &= ~(1<<OCIE1C)) : (TIMSK3 &= ~(1<<OCIE3C)) )
#       define Timer16_capture_it_enable()      ((timer16_selected==TIMER16_1)? \
                                                    (TIMSK1 |= (1<<ICIE1)) : (TIMSK3 |= (1<<ICIE3)) )
#       define Timer16_capture_it_disable()     ((timer16_selected==TIMER16_1)? \
                                                    (TIMSK1 &= ~(1<<ICIE1)) : (TIMSK3 &= ~(1<<ICIE3)) )
#       define Timer16_get_overflow_it_mask()   ((timer16_selected==TIMER16_1)? \
                                                    ((TIMSK1 & (1<<TOIE1)) >> TOIE1) : ((TIMSK3 & (1<<TOIE3)) >> TOIE3) )
#       define Timer16_get_compare_a_it_mask()  ((timer16_selected==TIMER16_1)? \
                                                    ((TIMSK1 & (1<<OCIE1A)) >> OCIE1A) : ((TIMSK3 & (1<<OCIE3A)) >> OCIE3A) )
#       define Timer16_get_compare_b_it_mask()  ((timer16_selected==TIMER16_1)? \
                                                    ((TIMSK1 & (1<<OCIE1B)) >> OCIE1B) : ((TIMSK3 & (1<<OCIE3B)) >> OCIE3B) )
#       define Timer16_get_compare_c_it_mask()  ((timer16_selected==TIMER16_1)? \
                                                    ((TIMSK1 & (1<<OCIE1C)) >> OCIE1C) : ((TIMSK3 & (1<<OCIE3C)) >> OCIE3C) )
#       define Timer16_get_capture_it_mask()    ((timer16_selected==TIMER16_1)? \
                                                    ((TIMSK1 & (1<<ICIE1)) >> ICIE1) : ((TIMSK3 & (1<<ICIE3)) >> ICIE3) )
    // ----------
#       define Timer16_clear_overflow_it()      ((timer16_selected==TIMER16_1)? \
                                                    (TIFR1 |= (1<<TOV1)) : (TIFR3 |= (1<<TOV3)) )
#       define Timer16_clear_compare_a_it()     ((timer16_selected==TIMER16_1)? \
                                                    (TIFR1 |= (1<<OCF1A)) : (TIFR3 |= (1<<OCF3A)) )
#       define Timer16_clear_compare_b_it()     ((timer16_selected==TIMER16_1)? \
                                                    (TIFR1 |= (1<<OCF1B)) : (TIFR3 |= (1<<OCF3B)) )
#       define Timer16_clear_compare_c_it()     ((timer16_selected==TIMER16_1)? \
                                                    (TIFR1 |= (1<<OCF1C)) : (TIFR3 |= (1<<OCF3C)) )
#       define Timer16_clear_capture_it()       ((timer16_selected==TIMER16_1)? \
                                                    (TIFR1 |= (1<<ICF1)) : (TIFR3 |= (1<<ICF3)) )
#       define Timer16_get_overflow_it()        ((timer16_selected==TIMER16_1)? \
                                                    ((TIFR1 & (1<<TOV1)) >> TOV1) : ((TIFR3 & (1<<TOV3)) >> TOV3) )
#       define Timer16_get_compare_a_it()       ((timer16_selected==TIMER16_1)? \
                                                    ((TIFR1 & (1<<OCF1A)) >> OCF1A) : ((TIFR3 & (1<<OCF3A)) >> OCF3A) )
#       define Timer16_get_compare_b_it()       ((timer16_selected==TIMER16_1)? \
                                                    ((TIFR1 & (1<<OCF1B)) >> OCF1B) : ((TIFR3 & (1<<OCF3B)) >> OCF3B) )
#       define Timer16_get_compare_c_it()       ((timer16_selected==TIMER16_1)? \
                                                    ((TIFR1 & (1<<OCF1C)) >> OCF1C) : ((TIFR3 & (1<<OCF3C)) >> OCF3C) )
#       define Timer16_get_capture_it()         ((timer16_selected==TIMER16_1)? \
                                                    ((TIFR1 & (1<<ICF1)) >> ICF1) : ((TIFR3 & (1<<ICF3)) >> ICF3) )

    #else
        #error USE_TIMER16 definition is not referenced in "timer16_drv.h" file
#endif

//_____ T I M E R   D E F I N I T I O N S ______________________________________

    // ---------- Pre-definitions for "conf" field for Timer16_set(get)_mode_output_x(conf) macros
#define TIMER16_COMP_MODE_NORMAL    (0)
#define TIMER16_COMP_MODE_TOGGLE    (1)
#define TIMER16_COMP_MODE_CLEAR_OC  (2)
#define TIMER16_COMP_MODE_SET_OC    (3)
#define TIMER16_COMP_MODE_MASK_A    (3<<COM1A0)
#define TIMER16_COMP_MODE_MASK_B    (3<<COM1B0)
#define TIMER16_COMP_MODE_MASK_C    (3<<COM1C0)
    // ---------- Pre-definitions for "conf" field for Timer16_set_waveform_mode(conf) macro
#define TIMER16_WGM_NORMAL          (0)
#define TIMER16_WGM_CTC_OCR         (4)
#define TIMER16_WGM_CTC_ICR         (12)
#define TIMER16_WGM_PWM_PC8         (1)
#define TIMER16_WGM_PWM_PC9         (2)
#define TIMER16_WGM_PWM_PC10        (3)
#define TIMER16_WGM_PWM_PC_ICR      (10)
#define TIMER16_WGM_PWM_PC_OCR      (11)
#define TIMER16_WGM_PWM_PFC_ICR     (8)
#define TIMER16_WGM_PWM_PFC_OCR     (9)
#define TIMER16_WGM_FAST_PWM8       (5)
#define TIMER16_WGM_FAST_PWM9       (6)
#define TIMER16_WGM_FAST_PWM10      (7)
#define TIMER16_WGM_FAST_PWM_ICR    (14)
#define TIMER16_WGM_FAST_PWM_OCR    (15)
#define TIMER16_WGM_RA_MASK         (3<<WGM10)
#define TIMER16_WGM_RB_MASK         (3<<WGM12)
    // ---------- Pre-definitions for "value" field for Timer16_set_clock(value) macro
#define TIMER16_NO_CLOCK                 (0)
#define TIMER16_CLKIO_BY_1               (1)
#define TIMER16_CLKIO_BY_8               (2)
#define TIMER16_CLKIO_BY_64              (3)
#define TIMER16_CLKIO_BY_256             (4)
#define TIMER16_CLKIO_BY_1024            (5)
#define TIMER16_EXT_CLOCK_FALLING_EDGE   (6)
#define TIMER16_EXT_CLOCK_RISING_EDGE    (7)
#define TIMER16_CLK_MASK                 (7<<CS10)


//_____ D E C L A R A T I O N S ________________________________________________

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
//!
extern  U16 timer16_get_counter(void);

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
//!
extern  U16 timer16_get_capture(void);

//______________________________________________________________________________

#endif  // _TIMER16_DRV_H_
























