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
                    .syntax  unified
                    .thumb

// ============================================================================

                    .section .data.lpc13xx, "w"

                    .global  LPC13XX_SYSCON
                    .global  LPC13XX_IOCON
                    .global  LPC13XX_GPIO0
                    .global  LPC13XX_GPIO1
                    .global  LPC13XX_GPIO2
                    .global  LPC13XX_GPIO3
                    .global  LPC13XX_UART
                    .global  LPC13XX_I2C
                    .global  LPC13XX_FMC

                    .global  LPC13XX_SYSTICK

LPC13XX_SYSCON:     .word    0x40048000 // Figure 2: memory mapping => AHB     peripheral bus
LPC13XX_IOCON:      .word    0x40044000 // Figure 2: memory mapping => AHB     peripheral bus
LPC13XX_GPIO0:      .word    0x50000000 // Figure 2: memory mapping => AHB     peripheral bus, GPIO0 => PIO0
LPC13XX_GPIO1:      .word    0x50010000 // Figure 2: memory mapping => AHB     peripheral bus, GPIO1 => PIO1
LPC13XX_GPIO2:      .word    0x50020000 // Figure 2: memory mapping => AHB     peripheral bus, GPIO2 => PIO2
LPC13XX_GPIO3:      .word    0x50030000 // Figure 2: memory mapping => AHB     peripheral bus, GPIO3 => PIO3
LPC13XX_UART:       .word    0x40008000 // Figure 2: memory mapping => AHB     peripheral bus
LPC13XX_I2C:        .word    0x40000000 // Figure 2: memory mapping => AHB     peripheral bus
LPC13XX_FMC:        .word    0x4003C000 // Figure 2: memory mapping => AHB     peripheral bus

LPC13XX_SYSTICK:    .word    0xE000E000 // Figure 2: memory mapping => private peripheral bus

#if defined( LPC13XX_SYSTICK_56BIT )
                    .global  lpc13xx_tsc

lpc13xx_tsc:        .word    0xFFFFFFFF
#endif

// ----------------------------------------------------------------------------

                    .section .text.lpc13xx, "x"

                    .thumb_func  
                    .global  lpc13xx_irq_enable
           
lpc13xx_irq_enable: cpsie    I          // set CPSR[ I ] = 0

                    bx       lr         // return
     
                    .thumb_func  
                    .global  lpc13xx_irq_unable

lpc13xx_irq_unable: cpsid    I          // set CPSR[ I ] = 1

                    bx       lr         // return

// ============================================================================

