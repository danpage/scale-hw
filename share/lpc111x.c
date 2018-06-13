/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

/* Throughout the following, note that all Chapter, Section, Page references 
 * are to the lpc111x user manual at
 *
 * http://www.nxp.com/docs/en/user-guide/UM10398.pdf
 */

#include "lpc111x.h"

lpc111x_syscon_dev_t* LPC111X_SYSCON = ( lpc111x_syscon_dev_t* )( 0x40048000 ); // Figure 6: memory mapping
lpc111x_iocon_dev_t*  LPC111X_IOCON  = ( lpc111x_iocon_dev_t*  )( 0x40044000 ); // Figure 6: memory mapping
lpc111x_gpio_dev_t*   LPC111X_GPIO0  = ( lpc111x_gpio_dev_t*   )( 0x50000000 ); // Figure 6: memory mapping, GPIO0 => PIO0
lpc111x_gpio_dev_t*   LPC111X_GPIO1  = ( lpc111x_gpio_dev_t*   )( 0x50010000 ); // Figure 6: memory mapping, GPIO1 => PIO1
lpc111x_gpio_dev_t*   LPC111X_GPIO2  = ( lpc111x_gpio_dev_t*   )( 0x50020000 ); // Figure 6: memory mapping, GPIO2 => PIO2
lpc111x_gpio_dev_t*   LPC111X_GPIO3  = ( lpc111x_gpio_dev_t*   )( 0x50030000 ); // Figure 6: memory mapping, GPIO3 => PIO3
lpc111x_uart_dev_t*   LPC111X_UART   = ( lpc111x_uart_dev_t*   )( 0x40008000 ); // Figure 6: memory mapping
lpc111x_i2c_dev_t*    LPC111X_I2C    = ( lpc111x_i2c_dev_t*    )( 0x40000000 ); // Figure 6: memory mapping
lpc111x_fmc_dev_t*    LPC111X_FMC    = ( lpc111x_fmc_dev_t*    )( 0x4003C000 ); // Figure 6: memory mapping
