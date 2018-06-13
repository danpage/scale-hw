/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

/* Throughout the following, note that all Chapter, Section, Page references 
 * are to the lpc13xx user manual at
 *
 * http://www.nxp.com/docs/en/user-guide/UM10375.pdf
 */

#include "lpc13xx.h"

lpc13xx_syscon_dev_t* LPC13XX_SYSCON = ( lpc13xx_syscon_dev_t* )( 0x40048000 ); // Figure 2: memory mapping
lpc13xx_iocon_dev_t*  LPC13XX_IOCON  = ( lpc13xx_iocon_dev_t*  )( 0x40044000 ); // Figure 2: memory mapping
lpc13xx_gpio_dev_t*   LPC13XX_GPIO0  = ( lpc13xx_gpio_dev_t*   )( 0x50000000 ); // Figure 2: memory mapping, GPIO0 => PIO0
lpc13xx_gpio_dev_t*   LPC13XX_GPIO1  = ( lpc13xx_gpio_dev_t*   )( 0x50010000 ); // Figure 2: memory mapping, GPIO1 => PIO1
lpc13xx_gpio_dev_t*   LPC13XX_GPIO2  = ( lpc13xx_gpio_dev_t*   )( 0x50020000 ); // Figure 2: memory mapping, GPIO2 => PIO2
lpc13xx_gpio_dev_t*   LPC13XX_GPIO3  = ( lpc13xx_gpio_dev_t*   )( 0x50030000 ); // Figure 2: memory mapping, GPIO3 => PIO3
lpc13xx_uart_dev_t*   LPC13XX_UART   = ( lpc13xx_uart_dev_t*   )( 0x40008000 ); // Figure 2: memory mapping
lpc13xx_i2c_dev_t*    LPC13XX_I2C    = ( lpc13xx_i2c_dev_t*    )( 0x40000000 ); // Figure 2: memory mapping
lpc13xx_fmc_dev_t*    LPC13XX_FMC    = ( lpc13xx_fmc_dev_t*    )( 0x4003C000 ); // Figure 2: memory mapping
