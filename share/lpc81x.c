/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

/* Throughout the following, note that all Chapter, Section, Page references 
 * are to the lpc81x user manual at
 *
 * http://www.nxp.com/docs/en/user-guide/UM10601.pdf
 */

#include "lpc81x.h"

lpc81x_syscon_dev_t* LPC81X_SYSCON = ( lpc81x_syscon_dev_t* )( 0x40048000 ); // Figure 2: memory mapping
lpc81x_iocon_dev_t*  LPC81X_IOCON  = ( lpc81x_iocon_dev_t*  )( 0x40044000 ); // Figure 2: memory mapping
lpc81x_gpio_dev_t*   LPC81X_GPIO   = ( lpc81x_gpio_dev_t*   )( 0xA0000000 ); // Figure 2: memory mapping
lpc81x_swm_dev_t*    LPC81X_SWM    = ( lpc81x_swm_dev_t*    )( 0x4000C000 ); // Figure 2: memory mapping
lpc81x_uart_dev_t*   LPC81X_UART0  = ( lpc81x_uart_dev_t*   )( 0x40064000 ); // Figure 2: memory mapping
lpc81x_uart_dev_t*   LPC81X_UART1  = ( lpc81x_uart_dev_t*   )( 0x40068000 ); // Figure 2: memory mapping
lpc81x_uart_dev_t*   LPC81X_UART2  = ( lpc81x_uart_dev_t*   )( 0x4006C000 ); // Figure 2: memory mapping
lpc81x_i2c_dev_t*    LPC81X_I2C    = ( lpc81x_i2c_dev_t*    )( 0x40050000 ); // Figure 2: memory mapping
lpc81x_fmc_dev_t*    LPC81X_FMC    = ( lpc81x_fmc_dev_t*    )( 0x40040000 ); // Figure 2: memory mapping
