//******************************************************************************
//! @file $RCSfile: uart_drv.h,v $
//!
//! Copyright (c) 2007 Atmel.
//!
//! Use of this program is subject to Atmel's End User License Agreement.
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains the prototypes and the macros of the
//!        low level functions (drivers) of:
//!             - Both UARTs
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

#ifndef _UART_DRV_H_
#define _UART_DRV_H_

//_____ I N C L U D E S ________________________________________________________

#include "config.h"

//_____ D E F I N I T I O N S __________________________________________________

#ifndef FOSC
#  error  You must define FOSC in "config.h" file
#endif
    // ----------
#ifndef UART_AUTOBAUD               // Auto dectection of UART BaudRate
#define UART_AUTOBAUD   0
#endif
#ifndef VARIABLE_UART_BAUDRATE      // UART BaudRate can change during session
#define VARIABLE_UART_BAUDRATE   -1
#endif
    // ----------
#ifndef UART_BAUDRATE
#define UART_BAUDRATE   VARIABLE_UART_BAUDRATE
#endif
    /* ---------- BAUDRATE Examples ----------
    if FOSC == 16000        //!< Fclkio = 16 MHz
         UBRR_1K2   0x0680     //!< --   1k2 bps, error= 0.0%,
         UBRR_2K4   0x0340     //!< --   2k4 bps, error= 0.0%, U2Xn=1
         UBRR_4K8   0x01A0     //!< --   4k8 bps, error=-0.1%, U2Xn=1
         UBRR_9K6   0x00CF     //!< --   9k6 bps, error= 0.2%, U2Xn=1
         UBRR_14K4  0x008A     //!< --  14k4 bps, error=-0.1%, U2Xn=1
         UBRR_19K2  0x0067     //!< --  19k2 bps, error= 0.2%, U2Xn=1
         UBRR_28K8  0x0044     //!< --  28k8 bps, error= 0.6%, U2Xn=1
         UBRR_38K4  0x0033     //!< --  38k4 bps, error= 0.2%, U2Xn=1
         UBRR_57K6  0x0022     //!< --  57k6 bps, error=-0.8%, U2Xn=1
         UBRR_76K8  0x0019     //!< --  76k8 bps, error= 0.2%, U2Xn=1
         UBRR_115K2 0x0010     //!< -- 115k2 bps, error= 2.1%, U2Xn=1
    if FOSC == 12000        //!< Fclkio = 12 MHz
         UBRR_1K2   0x04E0     //!< --   1k2 bps, error= 0.0%, U2Xn=1
         UBRR_2K4   0x0270     //!< --   2k4 bps, error= 0.0%, U2Xn=1
         UBRR_4K8   0x0138     //!< --   4k8 bps, error=-0.2%, U2Xn=1
         UBRR_9K6   0x009B     //!< --   9k6 bps, error= 0.2%, U2Xn=1
         UBRR_14K4  0x0067     //!< --  14k4 bps, error= 0.2%, U2Xn=1
         UBRR_19K2  0x004D     //!< --  19k2 bps, error= 0.2%, U2Xn=1
         UBRR_28K8  0x0033     //!< --  28k8 bps, error= 0.2%, U2Xn=1
         UBRR_38K4  0x0026     //!< --  38k4 bps, error= 0.2%, U2Xn=1
         UBRR_57K6  0x0019     //!< --  57k6 bps, error= 0.2%, U2Xn=1
         UBRR_76K8  0x0013     //!< --  76k8 bps, error=-2.5%, U2Xn=1
         UBRR_115K2 0x000C     //!< -- 115k2 bps, error= 0.2%, U2Xn=1
    if FOSC == 8000         //!< Fclkio = 8 MHz
         UBRR_1K2   0x0340     //!< --   1k2 bps, error= 0.0%, U2Xn=1
         UBRR_2K4   0x01A0     //!< --   2k4 bps, error=-0.1%, U2Xn=1
         UBRR_4K8   0x00CF     //!< --   4k8 bps, error= 0.2%, U2Xn=1
         UBRR_9K6   0x0067     //!< --   9k6 bps, error= 0.2%, U2Xn=1
         UBRR_14K4  0x0044     //!< --  14k4 bps, error= 0.6%, U2Xn=1
         UBRR_19K2  0x0033     //!< --  19k2 bps, error= 0.2%, U2Xn=1
         UBRR_28K8  0x0022     //!< --  28k8 bps, error=-0.8%, U2Xn=1
         UBRR_38K4  0x0019     //!< --  38k4 bps, error= 0.2%, U2Xn=1
         UBRR_57K6  0x0010     //!< --  57k6 bps, error= 2.1%, U2Xn=1
         UBRR_76K8  0x000C     //!< --  76k8 bps, error= 0.2%, U2Xn=1
         UBRR_115K2 0x0008     //!< -- 115k2 bps, error=-3.5%, U2Xn=1   */

    // ----------
#ifndef UART_0               // UART 0 Defintion
#define UART_0      0x00
#endif
#ifndef UART_1               // UART 0 Defintion
#define UART_1      0x01
#endif
#ifndef BOTH_UART            // Both the UARTs Defintion
#define BOTH_UART   0xFF
#endif
#ifndef NO_UART              // No UART Defintion (do not use it in "config.h")
#define NO_UART     0xFE
#endif
    // ----------
#ifndef USE_UART
#       error You must define USE_UART to UART_0 or UART_1 or BOTH_UART in "board.h" file

#   elif USE_UART == UART_0
    //!< ==============================
    //!< ------ Only UART_0 used ------
    //!< ==============================
        // Macros
#       define Uart_set_ubrr(baudrate) { UBRR0H = (U8)((((((((U32)FOSC*1000)<<1)/((U32)baudrate*8))+1)>>1)-1)>>8); \
                                         UBRR0L = (U8)(( (((((U32)FOSC*1000)<<1)/((U32)baudrate*8))+1)>>1)-1)    ; \
                                         UCSR0A |=  (1<<U2X0); }
#       define Uart_clear()            { UCSR0B=0; UCSR0C=UDR0; UCSR0A=0x40; UCSR0C=0x06; UBRR0H=0; UBRR0L=0; }
#       define Uart_hw_init(config)    { UCSR0A |=  (1<<UDRE0);                                    \
                                         UCSR0B &= (~(1<<UCSZ02)); UCSR0B |= config & (1<<UCSZ02); \
                                         UCSR0C  = config & ((3<<UPM00) | (1<<USBS0));             \
                                         UCSR0C |= ((config & (3<<(UCSZ00-1))) << UCSZ00);         }
#       define Uart_enable()           ( UCSR0B |=  ((1<<RXEN0) | (1<<TXEN0)) )
#       define Uart_disable()          ( UCSR0B &= ~((1<<RXEN0) | (1<<TXEN0)) )
#       define Uart_tx_ready()        (((UCSR0A &   1<<UDRE0)) >> UDRE0 )
#       define Uart_send_byte(ch)      ( UDR0 = ch)
#       define Uart_rx_ready()         ((UCSR0A & (1<<RXC0)) >> RXC0 )
#       define Uart_get_byte()         ( UDR0 )
        // Void
#       define Uart_select(uart_num)
#       define Uart_set_tx_busy()
#       define Uart_ack_rx_byte()
        // Physical UART Definition
#       define UART_0_PORT_IN       (PINE)
#       define UART_0_PORT_DIR      (DDRE)
#       define UART_0_PORT_OUT      (PORTE)
#       define UART_0_INPUT_PIN     (0)
#       define UART_0_OUTPUT_PIN    (1)
#       define Uart_rx_in_pu_on()   { UART_0_PORT_DIR &= ~(1<<UART_0_INPUT_PIN); UART_0_PORT_OUT  |=  (1<<UART_0_INPUT_PIN); }   // RxD in input + pull-up
#       define Uart_rx_in_pu_off()  { UART_0_PORT_DIR &= ~(1<<UART_0_INPUT_PIN); UART_0_PORT_OUT  &= ~(1<<UART_0_INPUT_PIN); }   // RxD in input no pull-up
#       define Uart_rx_get_data()   ((UART_0_PORT_IN  &   (1<<UART_0_INPUT_PIN)) >> UART_0_INPUT_PIN )                          // Read RxD pin
#       define Uart_rx_get_3_data() ((UART_0_PORT_IN  & UART_0_PORT_IN  & UART_0_PORT_IN  & (1<<UART_0_INPUT_PIN)) >> UART_0_INPUT_PIN )  // Read 3 times RxD pin
#       define Uart_tx_in_pu_on()   { UART_0_PORT_DIR &= ~(1<<UART_0_OUTPUT_PIN); UART_0_PORT_OUT  |=  (1<<UART_0_OUTPUT_PIN); }   // TxD in input + pull-up
#       define Uart_tx_in_pu_off()  { UART_0_PORT_DIR &= ~(1<<UART_0_OUTPUT_PIN); UART_0_PORT_OUT  &= ~(1<<UART_0_OUTPUT_PIN); }   // TxD in input no pull-up
#       define Uart_set_tx()        { UART_0_PORT_DIR |=  (1<<UART_0_INPUT_PIN); UART_0_PORT_OUT |=  (1<<UART_0_INPUT_PIN); }   // TxD in output + set pin
#       define Uart_clear_tx()      { UART_0_PORT_DIR |=  (1<<UART_0_INPUT_PIN); UART_0_PORT_OUT &= ~(1<<UART_0_INPUT_PIN); }   // TxD in output + clear pin

#   elif USE_UART == UART_1
    //!< ==============================
    //!< ------ Only UART_1 used ------
    //!< ==============================
        // Macros
#       define Uart_set_ubrr(baudrate) { UBRR1H = (U8)((((((((U32)FOSC*1000)<<1)/((U32)baudrate*8))+1)>>1)-1)>>8); \
                                         UBRR1L = (U8)(( (((((U32)FOSC*1000)<<1)/((U32)baudrate*8))+1)>>1)-1)    ; \
                                         UCSR1A |=  (1<<U2X1); }
#       define Uart_clear()            { UCSR1B=0; UCSR1C=UDR1; UCSR1A=0x40; UCSR1C=0x06; UBRR1H=0; UBRR1L=0; }
#       define Uart_hw_init(config)    { UCSR1A |=  (1<<UDRE1);                                    \
                                         UCSR1B &= (~(1<<UCSZ12)); UCSR0B |= config & (1<<UCSZ12); \
                                         UCSR1C  = config & ((3<<UPM10) | (1<<USBS1));             \
                                         UCSR1C |= ((config & (3<<(UCSZ10-1))) << UCSZ10);         }
#       define Uart_enable()           ( UCSR1B |=  ((1<<RXEN1) | (1<<TXEN1)) )
#       define Uart_disable()          ( UCSR1B &= ~((1<<RXEN1) | (1<<TXEN1)) )
#       define Uart_tx_ready()        (((UCSR1A &   1<<UDRE1)) >> UDRE1 )
#       define Uart_send_byte(ch)      ( UDR1 = ch)
#       define Uart_rx_ready()         ((UCSR1A & (1<<RXC1)) >> RXC1 )
#       define Uart_get_byte()         ( UDR1 )
        // Void
#       define Uart_select(uart_num)
#       define Uart_set_tx_busy()
#       define Uart_ack_rx_byte()
        // Physical part, pull-up on Rx/Tx
#       define UART_1_PORT_IN       (PIND)
#       define UART_1_PORT_DIR      (DDRD)
#       define UART_1_PORT_OUT      (PORTD)
#       define UART_1_INPUT_PIN     (2)
#       define UART_1_OUTPUT_PIN    (3)
#       define Uart_rx_in_pu_on()   { UART_1_PORT_DIR &= ~(1<<UART_1_INPUT_PIN); UART_1_PORT_OUT  |=  (1<<UART_1_INPUT_PIN); }   // RxD in input + pull-up
#       define Uart_rx_in_pu_off()  { UART_1_PORT_DIR &= ~(1<<UART_1_INPUT_PIN); UART_1_PORT_OUT  &= ~(1<<UART_1_INPUT_PIN); }   // RxD in input no pull-up
#       define Uart_rx_get_data()   ((UART_1_PORT_IN  &   (1<<UART_1_INPUT_PIN)) >> UART_1_INPUT_PIN )                          // Read RxD pin
#       define Uart_rx_get_3_data() ((UART_1_PORT_IN  & UART_1_PORT_IN  & UART_1_PORT_IN  & (1<<UART_1_INPUT_PIN)) >> UART_1_INPUT_PIN )  // Read 3 times RxD pin
#       define Uart_tx_in_pu_on()   { UART_1_PORT_DIR &= ~(1<<UART_1_OUTPUT_PIN); UART_1_PORT_OUT  |=  (1<<UART_1_OUTPUT_PIN); }   // TxD in input + pull-up
#       define Uart_tx_in_pu_off()  { UART_1_PORT_DIR &= ~(1<<UART_1_OUTPUT_PIN); UART_1_PORT_OUT  &= ~(1<<UART_1_OUTPUT_PIN); }   // TxD in input no pull-up
#       define Uart_set_tx()        { UART_1_PORT_DIR |=  (1<<UART_1_INPUT_PIN); UART_1_PORT_OUT |=  (1<<UART_1_INPUT_PIN); }   // TxD in output + set pin
#       define Uart_clear_tx()      { UART_1_PORT_DIR |=  (1<<UART_1_INPUT_PIN); UART_1_PORT_OUT &= ~(1<<UART_1_INPUT_PIN); }   // TxD in output + clear pin

#   elif USE_UART  == BOTH_UART
    //!< =====================================
    //!< ------ Both the UARTs are used ------
    //!< =====================================
        extern U8 uart_selected;      // $$$-- EXTERNAL DECLARATION --$$$
        #define Uart_select(uart_num)   (uart_selected=uart_num)
        // Macros
#       define Uart_set_ubrr(baudrate)  ((uart_selected==UART_0)? \
                                            (UBRR0H  = (U8)((((((((U32)FOSC*1000)<<1)/((U32)baudrate*8))+1)>>1)-1)>>8), \
                                             UBRR0L  = (U8)(( (((((U32)FOSC*1000)<<1)/((U32)baudrate*8))+1)>>1)-1)    , \
                                             UCSR0A |=  (1<<U2X0) )\
                                            : \
                                            (UBRR1H  = (U8)((((((((U32)FOSC*1000)<<1)/((U32)baudrate*8))+1)>>1)-1)>>8), \
                                             UBRR1L  = (U8)(( (((((U32)FOSC*1000)<<1)/((U32)baudrate*8))+1)>>1)-1)    , \
                                             UCSR1A |=  (1<<U2X1) )                                                     )
#       define Uart_clear()             ((uart_selected==UART_0)? \
                                            (UCSR0B=0, UCSR0C=UDR0, UCSR0A=0x40, UCSR0C=0x06, UBRR0H=0, UBRR0L=0) \
                                            : \
                                            (UCSR1B=0, UCSR1C=UDR1, UCSR1A=0x40, UCSR1C=0x06, UBRR1H=0, UBRR1L=0) )
#       define Uart_hw_init(config)     ((uart_selected==UART_0)? \
                                            (UCSR0A |=  (1<<UDRE0),                             \
                                             UCSR0B &= (~(1<<UCSZ02)),                          \
                                             UCSR0B |= config & (1<<UCSZ02),                    \
                                             UCSR0C  = config & ((3<<UPM00) | (1<<USBS0)),      \
                                             UCSR0C |= ((config & (3<<(UCSZ00-1))) << UCSZ00) ) \
                                            : \
                                            (UCSR1A |=  (1<<UDRE1),                                    \
                                             UCSR1B &= (~(1<<UCSZ12)),                          \
                                             UCSR1B |= config & (1<<UCSZ12),                    \
                                             UCSR1C  = config & ((3<<UPM10) | (1<<USBS1)),      \
                                             UCSR1C |= ((config & (3<<(UCSZ10-1))) << UCSZ10) ) )
#       define Uart_enable()            ((uart_selected==UART_0)? \
                                            (UCSR0B |=  ((1<<RXEN0) | (1<<TXEN0))) \
                                            : \
                                            (UCSR1B |=  ((1<<RXEN1) | (1<<TXEN1))) )
#       define Uart_disable()           ((uart_selected==UART_0)? \
                                            (UCSR0B &= ~((1<<RXEN0) | (1<<TXEN0))) \
                                            : \
                                            (UCSR1B &= ~((1<<RXEN1) | (1<<TXEN1))) )
#       define Uart_tx_ready()          ((uart_selected==UART_0)? \
                                            (((UCSR0A &   1<<UDRE0)) >> UDRE0) \
                                            : \
                                            (((UCSR1A &   1<<UDRE1)) >> UDRE1) )
#       define Uart_send_byte(ch)       ((uart_selected==UART_0)? (UDR0 = ch) : (UDR1 = ch) )
#       define Uart_rx_ready()          ((uart_selected==UART_0)? \
                                            ((UCSR0A & (1<<RXC0)) >> RXC0) \
                                            : \
                                            ((UCSR1A & (1<<RXC1)) >> RXC1) )
#       define Uart_get_byte()          ((uart_selected==UART_0)? (UDR0) : (UDR1) )
        // Void
#       define Uart_set_tx_busy()
#       define Uart_ack_rx_byte()
        // Physical part, pull-up on Rx/Tx
#       define UART_0_PORT_IN           (PINE)
#       define UART_1_PORT_IN           (PIND)
#       define UART_0_PORT_DIR          (DDRE)
#       define UART_1_PORT_DIR          (DDRD)
#       define UART_0_PORT_OUT          (PORTE)
#       define UART_1_PORT_OUT          (PORTD)
#       define UART_0_INPUT_PIN         (0)
#       define UART_1_INPUT_PIN         (2)
#       define UART_0_OUTPUT_PIN        (1)
#       define UART_1_OUTPUT_PIN        (3)
#       define Uart_rx_in_pu_on()       ((uart_selected==UART_0)? \
                                            (UART_0_PORT_DIR &= ~(1<<UART_0_INPUT_PIN), UART_0_PORT_OUT  |=  (1<<UART_0_INPUT_PIN) ) \
                                            : \
                                            (UART_1_PORT_DIR &= ~(1<<UART_1_INPUT_PIN), UART_1_PORT_OUT  |=  (1<<UART_1_INPUT_PIN) ) ) // RxD in input + pull-up
#       define Uart_rx_in_pu_off()      ((uart_selected==UART_0)? \
                                            (UART_0_PORT_DIR &= ~(1<<UART_0_INPUT_PIN), UART_0_PORT_OUT  &= ~(1<<UART_0_INPUT_PIN) ) \
                                            : \
                                            (UART_1_PORT_DIR &= ~(1<<UART_1_INPUT_PIN), UART_1_PORT_OUT  &= ~(1<<UART_1_INPUT_PIN) ) ) // RxD in input no pull-up
#       define Uart_rx_get_data()       ((uart_selected==UART_0)? \
                                            ((UART_0_PORT_IN  &   (1<<UART_0_INPUT_PIN)) >> UART_0_INPUT_PIN ) \
                                            : \
                                            ((UART_1_PORT_IN  &   (1<<UART_1_INPUT_PIN)) >> UART_1_INPUT_PIN ) )                      // Read RxD pin
#       define Uart_rx_get_3_data()     ((uart_selected==UART_0)? \
                                            ((UART_0_PORT_IN  & UART_0_PORT_IN  & UART_0_PORT_IN  & (1<<UART_0_INPUT_PIN)) >> UART_0_INPUT_PIN ) \
                                            : \
                                            ((UART_1_PORT_IN  & UART_1_PORT_IN  & UART_1_PORT_IN  & (1<<UART_1_INPUT_PIN)) >> UART_1_INPUT_PIN ) )    // Read 3 times RxD pin
#       define Uart_tx_in_pu_on()       ((uart_selected==UART_0)? \
                                            (UART_0_PORT_DIR &= ~(1<<UART_0_OUTPUT_PIN), UART_0_PORT_OUT  |=  (1<<UART_0_OUTPUT_PIN) ) \
                                            : \
                                            (UART_1_PORT_DIR &= ~(1<<UART_1_OUTPUT_PIN), UART_1_PORT_OUT  |=  (1<<UART_1_OUTPUT_PIN) ) ) // TxD in input + pull-up
#       define Uart_tx_in_pu_off()      ((uart_selected==UART_0)? \
                                            (UART_0_PORT_DIR &= ~(1<<UART_0_OUTPUT_PIN), UART_0_PORT_OUT  &= ~(1<<UART_0_OUTPUT_PIN) ) \
                                            : \
                                            (UART_1_PORT_DIR &= ~(1<<UART_1_OUTPUT_PIN), UART_1_PORT_OUT  &= ~(1<<UART_1_OUTPUT_PIN) ) ) // TxD in input no pull-up
#       define Uart_set_tx()            ((uart_selected==UART_0)? \
                                            (UART_0_PORT_DIR |=  (1<<UART_0_INPUT_PIN), UART_0_PORT_OUT |=  (1<<UART_0_INPUT_PIN) ) \
                                            : \
                                            (UART_1_PORT_DIR |=  (1<<UART_1_INPUT_PIN), UART_1_PORT_OUT |=  (1<<UART_1_INPUT_PIN) ) ) // TxD in output + set pin
#       define Uart_clear_tx()          ((uart_selected==UART_0)? \
                                            (UART_0_PORT_DIR |=  (1<<UART_0_INPUT_PIN), UART_0_PORT_OUT &= ~(1<<UART_0_INPUT_PIN) ) \
                                            : \
                                            (UART_1_PORT_DIR |=  (1<<UART_1_INPUT_PIN), UART_1_PORT_OUT &= ~(1<<UART_1_INPUT_PIN) ) ) // TxD in output + clear pin
#   else
#       error USE_UART definition is not referenced in "uart_drv.h" file
#endif

    // ---------- Pre-definitions for "Uart_hw_init(config)" macro.
#define CONF_9BIT_NOPAR_1STOP      ( (0<<UPM00) | (0<<USBS0) | (7<<(UCSZ00-1)) )
#define CONF_9BIT_NOPAR_2STOP      ( (0<<UPM00) | (1<<USBS0) | (7<<(UCSZ00-1)) )
#define CONF_9BIT_EVENPAR_1STOP    ( (2<<UPM00) | (0<<USBS0) | (7<<(UCSZ00-1)) )
#define CONF_9BIT_EVENPAR_2STOP    ( (2<<UPM00) | (1<<USBS0) | (7<<(UCSZ00-1)) )
#define CONF_9BIT_ODDPAR_1STOP     ( (3<<UPM00) | (0<<USBS0) | (7<<(UCSZ00-1)) )
#define CONF_9BIT_ODDPAR_2STOP     ( (3<<UPM00) | (1<<USBS0) | (7<<(UCSZ00-1)) )

#define CONF_8BIT_NOPAR_1STOP      ( (0<<UPM00) | (0<<USBS0) | (3<<(UCSZ00-1)) )
#define CONF_8BIT_NOPAR_2STOP      ( (0<<UPM00) | (1<<USBS0) | (3<<(UCSZ00-1)) )
#define CONF_8BIT_EVENPAR_1STOP    ( (2<<UPM00) | (0<<USBS0) | (3<<(UCSZ00-1)) )
#define CONF_8BIT_EVENPAR_2STOP    ( (2<<UPM00) | (1<<USBS0) | (3<<(UCSZ00-1)) )
#define CONF_8BIT_ODDPAR_1STOP     ( (3<<UPM00) | (0<<USBS0) | (3<<(UCSZ00-1)) )
#define CONF_8BIT_ODDPAR_2STOP     ( (3<<UPM00) | (1<<USBS0) | (3<<(UCSZ00-1)) )

#define CONF_7BIT_NOPAR_1STOP      ( (0<<UPM00) | (0<<USBS0) | (2<<(UCSZ00-1)) )
#define CONF_7BIT_NOPAR_2STOP      ( (0<<UPM00) | (1<<USBS0) | (2<<(UCSZ00-1)) )
#define CONF_7BIT_EVENPAR_1STOP    ( (2<<UPM00) | (0<<USBS0) | (2<<(UCSZ00-1)) )
#define CONF_7BIT_EVENPAR_2STOP    ( (2<<UPM00) | (1<<USBS0) | (2<<(UCSZ00-1)) )
#define CONF_7BIT_ODDPAR_1STOP     ( (3<<UPM00) | (0<<USBS0) | (2<<(UCSZ00-1)) )
#define CONF_7BIT_ODDPAR_2STOP     ( (3<<UPM00) | (1<<USBS0) | (2<<(UCSZ00-1)) )

#define CONF_6BIT_NOPAR_1STOP      ( (0<<UPM00) | (0<<USBS0) | (1<<(UCSZ00-1)) )
#define CONF_6BIT_NOPAR_2STOP      ( (0<<UPM00) | (1<<USBS0) | (1<<(UCSZ00-1)) )
#define CONF_6BIT_EVENPAR_1STOP    ( (2<<UPM00) | (0<<USBS0) | (1<<(UCSZ00-1)) )
#define CONF_6BIT_EVENPAR_2STOP    ( (2<<UPM00) | (1<<USBS0) | (1<<(UCSZ00-1)) )
#define CONF_6BIT_ODDPAR_1STOP     ( (3<<UPM00) | (0<<USBS0) | (1<<(UCSZ00-1)) )
#define CONF_6BIT_ODDPAR_2STOP     ( (3<<UPM00) | (1<<USBS0) | (1<<(UCSZ00-1)) )

#define CONF_5BIT_NOPAR_1STOP      ( (0<<UPM00) | (0<<USBS0) | (0<<(UCSZ00-1)) )
#define CONF_5BIT_NOPAR_2STOP      ( (0<<UPM00) | (1<<USBS0) | (0<<(UCSZ00-1)) )
#define CONF_5BIT_EVENPAR_1STOP    ( (2<<UPM00) | (0<<USBS0) | (0<<(UCSZ00-1)) )
#define CONF_5BIT_EVENPAR_2STOP    ( (2<<UPM00) | (1<<USBS0) | (0<<(UCSZ00-1)) )
#define CONF_5BIT_ODDPAR_1STOP     ( (3<<UPM00) | (0<<USBS0) | (0<<(UCSZ00-1)) )
#define CONF_5BIT_ODDPAR_2STOP     ( (3<<UPM00) | (1<<USBS0) | (0<<(UCSZ00-1)) )

//_____ M A C R O S ____________________________________________________________

//------------------------------------------------------------------------------
//! Set UART baudrate.
//! 1) If "UART_BAUDRATE == xxxxx" in "config.h" file, all the UARTs defined
//!    will be set with this value (computation made by the C pre-processor).
//! 2) If "UART_BAUDRATE == UART_AUTOBAUD" in "config.h" file, an automatic
//!    baudrate detection will be maded on all the UARTs defined.
//! 3) If "UART_BAUDRATE == VARIABLE_UART_BAUDRATE" in "config.h" file,
//!    this means that the application will set itself the baudrate value
//!    and this value could be changed during the session or/and the UARTs
//!    available into the microcontroller could have their own baudrate.
//!    To run auto baudrate detection on UART invoque "Uart_set_baudrate(0)".
//! 4) If "UART_BAUDRATE" is not defined in "config.h" file, "UART_BAUDRATE"
//!    will be defined and will be equals to "VARIABLE_UART_BAUDRATE". Then
//!    same behavior than in 3).
//!
//! @warning Write UBBR_H first then UBBR_L last, writting UBBR_L triggers
//!          an immediate update of the baud rate prescaler.
//!
//! The macro "Uart_set_baudrate(baudrate)" rounds to nearest the division
//! result (i.e. FOSC/BAUDRATE).
//!
//! U2Xnset to 1 for all type of computation.
//!
//! @param baudrate (baudrate==0 means automatic baudrate detection)
//------------------------------------------------------------------------------
#if UART_BAUDRATE == VARIABLE_UART_BAUDRATE
#   define Uart_set_baudrate(baudrate) ( uart_set_baudrate(baudrate) )
#elif UART_BAUDRATE == UART_AUTOBAUD
#   define Uart_set_baudrate(baudrate) ( uart_set_baudrate(0)        )
#else
#   define Uart_set_baudrate(baudrate) ( Uart_set_ubrr(UART_BAUDRATE))
#endif

//_____ D E C L A R A T I O N S ________________________________________________

// (c.f. above) extern char uart_selected;

#if ((UART_BAUDRATE == VARIABLE_UART_BAUDRATE) || (UART_BAUDRATE == UART_AUTOBAUD))
//------------------------------------------------------------------------------
//  @fn uart_set_baudrate
//!
//! This function programs the UBRRH register of both the UARTs from the
//! value passed as parameter or from an automatic timing detection if
//! there is some communication (activity) on the serial link. This function
//! is called by "Uart_set_baudrate(baudrate)" macro.
//! The function rounds to nearest the division result (i.e. FOSC/BAUDRATE).
//!
//! @warning Write UBBR_H first then UBBR_L last, writting UBBR_L triggers
//!          an immediate update of the baud rate prescaler.
//!
//! @param  !=0: baudrate value
//!         ==0: automatic baudrate detection
//!
//! @return Baudrate Status
//!         ==0: baudrate configuration failed
//!         ==1: baudrate performed
//!
extern  U8 uart_set_baudrate(U32 baudrate);
#endif

//------------------------------------------------------------------------------
//  @fn uart_rx_get_3_data
//!
//! This function returns the UART pin value sampled 3 times.
//!
//! @warning none
//!
//! @param  none
//!
//! @return UART pin value sampled 3 times
extern  Bool uart_rx_get_3_data (void);

//------------------------------------------------------------------------------
//  @fn uart_rx_get_data
//!
//! This function returns the UART pin value.
//!
//! @warning none
//!
//! @param  none
//!
//! @return UART pin value
extern  Bool uart_rx_get_data (void);

//______________________________________________________________________________

#endif  // _UART_DRV_H_

