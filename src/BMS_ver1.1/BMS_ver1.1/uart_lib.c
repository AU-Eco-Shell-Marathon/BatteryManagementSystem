//******************************************************************************
//! @file $RCSfile: uart_lib.c,v $
//!
//! Copyright (c) 2007 Atmel.
//!
//! Use of this program is subject to Atmel's End User License Agreement.
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains the library of functions of:
//!             - Both UARTs
//!             - AT90CAN128/64/32
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
#include "uart_lib.h"
#include "uart_drv.h"

//_____ D E F I N I T I O N S __________________________________________________

//******************************************************************************
//  @fn txt_logo_atmel
//!
//! Standard logo string + library version.
//!
//******************************************************************************
#ifndef REDUCED_UART_LIB
U8 txt_logo_atmel[]="\r\n\
        _____________________________\r\n\
      /___   ________________________|\r\n\
     ___  | |  ________    ____   _\r\n\
    /   | | | |  _   _ \\  |  __| | |\r\n\
   / /| | | | | | | | | | | |__  | |\r\n\
  / / | | | | | | | | | | |  __| | |\r\n\
 / /_ | | | | | | | | | | | |__  | |__\r\n\
/____||_| |_| |_| |_| |_| |____| |___/\r\n\
,-----------------------------------,\r\n\
| AT90CANxx Library Version 3.20   / (R)\r\n\
'---------------------------------'\r\n\n";

U8 txt_small_logo_atmel[]="--- AT90CANxx Library Version 3.20 ---\r\n";
#endif  // REDUCED_UART_LIB

//_____ F U N C T I O N S ______________________________________________________

//------------------------------------------------------------------------------
//  @fn uart_init
//!
//! UART peripheral initialization. Reset the UART, initialize the uart
//! mode, initialize the baudrate and enable the UART peripheral.
//!
//! @warning If autobaud, only one character is useful. If autobaud, one
//!          16-bit Timer is necessary.
//!
//! @param  Mode (c.f. predefined modes in "uart_drv.h" file)
//!         Baudrate (for fixed baudrate this param is not used)
//!
//! @return Baudrate Status
//!         ==0: research of timing failed
//!         ==1: baudrate performed
//!
//------------------------------------------------------------------------------
U8 uart_init (U8 mode, U32 baudrate)
{
    Uart_clear();       // Flush, Disable and Reset UART
    if (Uart_set_baudrate(baudrate) == 0) return 0;  //!<  c.f. macro in "uart_drv.h"
    Uart_hw_init(mode);     //!<  c.f. macro in "uart_drv.h"
    Uart_enable();          //!<  c.f. macro in "uart_drv.h"
    return (1);
}

//------------------------------------------------------------------------------
//  @fn uart_test_hit
//!
//! Check if something has been received on the UART peripheral.
//!
//! @warning none
//!
//! @param  none
//!
//! @return Baudrate Status
//!         ==0: Nothing has been received
//!         ==1: A character has been received
//!
//------------------------------------------------------------------------------
U8 uart_test_hit (void)
{
    return (Uart_rx_ready());
}

//------------------------------------------------------------------------------
//  @fn uart_putchar
//!
//! Send a character on the UART peripheral.
//!
//! @warning none
//!
//! @param  character to send
//!
//! @return character sent
//!
//------------------------------------------------------------------------------
U8 uart_putchar (U8 ch)
{
    while(!Uart_tx_ready());
    Uart_set_tx_busy();     // Set Busy flag before sending (always)
    Uart_send_byte(ch);
    return (ch);
}

//------------------------------------------------------------------------------
//  @fn uart_getchar
//!
//! Get a character from the UART peripheral.
//!
//! @warning none
//!
//! @param  none
//!
//! @return read (received) character on the UART
//!
//------------------------------------------------------------------------------
U8 uart_getchar (void)
{
    U8 ch;

    while(!Uart_rx_ready());
    ch = Uart_get_byte();
    Uart_ack_rx_byte();
    return ch;
}

//------------------------------------------------------------------------------
//  @fn uart_put_string
//!
//! Put a data-string on TX UART. The data-string is send up to null
//! character is found.
//!
//! @warning "uart_init()" must be performed before
//!
//! @param Pointer on U8 data-string
//!
//! @return (none)
//!
//------------------------------------------------------------------------------
#ifndef REDUCED_UART_LIB
void uart_put_string (U8 *data_string)
    {
    while(*data_string) uart_putchar (*data_string++);
    }
#endif  // REDUCED_UART_LIB

//------------------------------------------------------------------------------
//  @fn uart_mini_printf
//!
//! Minimal "PRINTF" with variable argument list. Write several variables
//! formatted by a format string to a file descriptor.
//! Example:
//! ========
//! { u8_toto = 0xAA;
//!   uart_mini_printf ("toto = %04d (0x%012X)\r\n", u8_toto, u8_toto);
//!   /*   Expected:     toto = 0170 (0x0000000000AA)   &  Cr+Lf       */ }
//!
//! @warning "uart_init()" must be performed before
//!
//! @param argument list
//!
//!     The format string is interpreted like this:
//!        ,---------------,---------------------------------------------------,
//!        | Any character | Output as is                                      |
//!        |---------------+---------------------------------------------------|
//!        |     %c:       | interpret argument as character                   |
//!        |     %s:       | interpret argument as pointer to string           |
//!        |     %d:       | interpret argument as decimal (signed) S16        |
//!        |     %ld:      | interpret argument as decimal (signed) S32        |
//!        |     %u:       | interpret argument as decimal (unsigned) U16      |
//!        |     %lu:      | interpret argument as decimal (unsigned) U32      |
//!        |     %x:       | interpret argument as hex U16 (lower case chars)  |
//!        |     %lx:      | interpret argument as hex U32 (lower case chars)  |
//!        |     %X:       | interpret argument as hex U16 (upper case chars)  |
//!        |     %lX:      | interpret argument as hex U32 (upper case chars)  |
//!        |     %%:       | print a percent ('%') character                   |
//!        '---------------'---------------------------------------------------'
//!
//!     Field width (in decimal) always starts with "0" and its maximum is
//!     given by "DATA_BUF_LEN" defined in "uart_lib.h".
//!        ,----------------------,-----------,--------------,-----------------,
//!        |       Variable       | Writting  |  Printing    |    Comment      |
//!        |----------------------+-----------+--------------|-----------------|
//!        |                      |   %x      | aa           |        -        |
//!        |  u8_xx = 0xAA        |   %04d    | 0170         |        -        |
//!        |                      |   %012X   | 0000000000AA |        -        |
//!        |----------------------+-----------+--------------|-----------------|
//!        | u16_xx = -5678       |   %010d   | -0000005678  |        -        |
//!        |----------------------+-----------+--------------|-----------------|
//!        | u32_xx = -4100000000 |   %011lu  | 00194967296  |        -        |
//!        |----------------------+-----------+--------------|-----------------|
//!        |          -           |   %8x     | 8x           | Writting error! |
//!        |----------------------+-----------+--------------|-----------------|
//!        |          -           |   %0s     | 0s           | Writting error! |
//!        '----------------------'-----------'--------------'-----------------'
//!       
//! Return: 0 = O.K.
//!
//------------------------------------------------------------------------------
#ifndef REDUCED_UART_LIB
U8 uart_mini_printf(char *format, ...)
{
    va_list arg_ptr;
    U8      *p,*sval;
    U8      u8_temp, n_sign, data_idx, min_size;
    U8      data_buf[DATA_BUF_LEN];
    S8      long_flag, alt_p_c;
    S8      s8_val;
    S16     s16_val;
    S32     s32_val;
    U16     u16_val;
    U32     u32_val;

    long_flag = FALSE;
    alt_p_c = FALSE;
    min_size = DATA_BUF_LEN-1;

    va_start(arg_ptr, format);   // make arg_ptr point to the first unnamed arg
    for (p = (U8 *) format; *p; p++)
    {
        if ((*p == '%') || (alt_p_c == TRUE))
        {
            p++;
        }
        else
        {
            uart_putchar(*p);
            alt_p_c = FALSE;
            long_flag = FALSE;
            continue;   // "switch (*p)" section skipped
        }
        switch (*p)
        {
            case 'c':
                if (long_flag == TRUE)      // ERROR: 'l' before any 'c'
                {
                    uart_putchar('l');
                    uart_putchar('c');
                }
                else
                {
                    s8_val = (S8)(va_arg(arg_ptr, int));    // s8_val = (S8)(va_arg(arg_ptr, S16));
                    uart_putchar((U8)(s8_val));
                }
                // Clean up
                min_size = DATA_BUF_LEN-1;
                alt_p_c = FALSE;
                long_flag = FALSE;
                break; // case 'c'
                
            case 's':
                if (long_flag == TRUE)      // ERROR: 'l' before any 's'
                {
                    uart_putchar('l');
                    uart_putchar('s');
                }
                else
                {
                    for (sval = va_arg(arg_ptr, U8 *); *sval; sval++)
                    {
                        uart_putchar(*sval);
                    }
                }
                // Clean up
                min_size = DATA_BUF_LEN-1;
                alt_p_c = FALSE;
                long_flag = FALSE;
                break;  // case 's'
                
            case 'l':  // It is not the number "ONE" but the lower case of "L" character
                if (long_flag == TRUE)      // ERROR: two consecutive 'l'
                {
                    uart_putchar('l');
                    alt_p_c = FALSE;
                    long_flag = FALSE;
                }
                else
                {
                    alt_p_c = TRUE;
                    long_flag = TRUE;
                }
                p--;
                break;  // case 'l'
                
            case 'd':
                n_sign  = FALSE;               
                for(data_idx = 0; data_idx < (DATA_BUF_LEN-1); data_idx++)
                {
                    data_buf[data_idx] = '0';
                }
                data_buf[DATA_BUF_LEN-1] = 0;
                data_idx = DATA_BUF_LEN - 2;
                if (long_flag)  // 32-bit
                {
                    s32_val = va_arg(arg_ptr, S32);
                    if (s32_val < 0)
                    {
                        n_sign = TRUE;
                        s32_val  = -s32_val;
                    }
                    while (1)
                    {
                        data_buf[data_idx] = s32_val % 10 + '0';
                        s32_val /= 10;
                        data_idx--;
						if (s32_val==0) break;
                   }
                }
                else  // 16-bit
                {
                    s16_val = (S16)(va_arg(arg_ptr, int)); // s16_val = va_arg(arg_ptr, S16);
                    if (s16_val < 0)
                    {
                        n_sign = TRUE;
                        s16_val  = -s16_val;
                    }
                    while (1)
                    {
                        data_buf[data_idx] = s16_val % 10 + '0';
                        s16_val /= 10;
                        data_idx--;
						if (s16_val==0) break;
                    }
                }
                if (n_sign) { uart_putchar('-'); }
                data_idx++;
                if (min_size < data_idx)
                {
                    data_idx = min_size;
                }
                uart_put_string (data_buf + data_idx);
                // Clean up
                min_size = DATA_BUF_LEN-1;
                alt_p_c = FALSE;
                long_flag = FALSE;
                break;  // case 'd'
                
            case 'u':
                for(data_idx = 0; data_idx < (DATA_BUF_LEN-1); data_idx++)
                {
                    data_buf[data_idx] = '0';
                }
                data_buf[DATA_BUF_LEN-1] = 0;
                data_idx = DATA_BUF_LEN - 2;
                if (long_flag)  // 32-bit
                {
                    u32_val = va_arg(arg_ptr, U32);
                    while (1)
                    {
                        data_buf[data_idx] = u32_val % 10 + '0';
                        u32_val /= 10;
                        data_idx--;
						if (u32_val==0) break;
                    }
                }
                else  // 16-bit
                {
                    u16_val = (U16)(va_arg(arg_ptr, int)); // u16_val = va_arg(arg_ptr, U16);
                    while (1)
                    {
                        data_buf[data_idx] = u16_val % 10 + '0';
                        data_idx--;
                        u16_val /= 10;
						if (u16_val==0) break;
                    }
                }
                data_idx++;
                if (min_size < data_idx)
                {
                    data_idx = min_size;
                }
                uart_put_string (data_buf + data_idx);
                // Clean up
                min_size = DATA_BUF_LEN-1;
                alt_p_c = FALSE;
                long_flag = FALSE;
                break;  // case 'u':
                
            case 'x':
            case 'X':
                for(data_idx = 0; data_idx < (DATA_BUF_LEN-1); data_idx++)
                {
                    data_buf[data_idx] = '0';
                }
                data_buf[DATA_BUF_LEN-1] = 0;
                data_idx = DATA_BUF_LEN - 2;
                if (long_flag)  // 32-bit
                { 
                    u32_val = va_arg(arg_ptr, U32);
                    while (u32_val)
                    {
                        u8_temp = (U8)(u32_val & 0x0F);
                        data_buf[data_idx] = (u8_temp < 10)? u8_temp+'0':u8_temp-10+(*p=='x'?'a':'A');
                        u32_val >>= 4;
                        data_idx--;
                    }
                }
                else  // 16-bit
                {
                    u16_val = (U16)(va_arg(arg_ptr, int)); // u16_val = va_arg(arg_ptr, U16);
                    while (u16_val)
                    {
                        u8_temp = (U8)(u16_val & 0x0F);
                        data_buf[data_idx] = (u8_temp < 10)? u8_temp+'0':u8_temp-10+(*p=='x'?'a':'A');
                        u16_val >>= 4;
                        data_idx--;
                    }
                }
                data_idx++;
                if (min_size < data_idx)
                {
                    data_idx = min_size;
                }
                uart_put_string (data_buf + data_idx);
                // Clean up
                min_size = DATA_BUF_LEN-1;
                alt_p_c = FALSE;
                long_flag = FALSE;
                break;  // case 'x' & 'X'
                
            case '0':   // Max allowed "min_size" 2 decimal digit, truncated to DATA_BUF_LEN-1.
                min_size = DATA_BUF_LEN-1;
                if (long_flag == TRUE)      // ERROR: 'l' before '0'
                {
                    uart_putchar('l');
                    uart_putchar('0');
                    // Clean up
                    alt_p_c = FALSE;
                    long_flag = FALSE;
                    break;
                }
                u8_temp = *++p;
                if ((u8_temp >='0') && (u8_temp <='9'))
                {
                    min_size = u8_temp & 0x0F;
                    u8_temp = *++p;
                    if ((u8_temp >='0') && (u8_temp <='9'))
                    {
                        min_size <<= 4;
                        min_size |= (u8_temp & 0x0F);
                        p++;
                    }
                    min_size = ((min_size & 0x0F) + ((min_size >> 4) *10));  // Decimal to hexa
                    if (min_size > (DATA_BUF_LEN-1))
                    {
                        min_size = (DATA_BUF_LEN-1);
                    }  // Truncation
                    min_size = DATA_BUF_LEN-1 - min_size;  // "min_size" formatted as "data_ix"
                }
                else      // ERROR: any "char" after '0'
                {
                    uart_putchar('0');
                    uart_putchar(*p);
                    // Clean up
                    alt_p_c = FALSE;
                    long_flag = FALSE;
                    break;
                }
                p-=2;
                alt_p_c = TRUE;
                // Clean up
                long_flag = FALSE;
                break;  // case '0'
                
            default:
                if (long_flag == TRUE)
                {
                    uart_putchar('l');
                }
                uart_putchar(*p);
                // Clean up
                min_size = DATA_BUF_LEN-1;
                alt_p_c = FALSE;
                long_flag = FALSE;
                break;  // default
                
        }   // switch (*p ...
        
    }   // for (p = ...
    
    va_end(arg_ptr);
    return 0;
}
#endif  // REDUCED_UART_LIB
