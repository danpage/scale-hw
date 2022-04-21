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
                    .syntax  unified
                    .thumb

// ============================================================================

                    .section .data.lpc81x, "w"

                    .global  LPC81X_SYSCON
                    .global  LPC81X_IOCON
                    .global  LPC81X_GPIO
                    .global  LPC81X_SWM
                    .global  LPC81X_UART0
                    .global  LPC81X_UART1
                    .global  LPC81X_UART2
                    .global  LPC81X_I2C
                    .global  LPC81X_FMC

                    .global  LPC81X_SYSTICK

LPC81X_SYSCON:      .word    0x40048000 // Figure 6: memory mapping => AHB     peripheral bus
LPC81X_IOCON:       .word    0x40044000 // Figure 6: memory mapping => AHB     peripheral bus
LPC81X_GPIO:        .word    0xA0000000 // Figure 6: memory mapping => AHB     peripheral bus
LPC81X_SWM:         .word    0x4000C000 // Figure 6: memory mapping => AHB     peripheral bus
LPC81X_UART0:       .word    0x40064000 // Figure 6: memory mapping => AHB     peripheral bus
LPC81X_UART1:       .word    0x40068000 // Figure 6: memory mapping => AHB     peripheral bus
LPC81X_UART2:       .word    0x4006C000 // Figure 6: memory mapping => AHB     peripheral bus
LPC81X_I2C:         .word    0x40050000 // Figure 6: memory mapping => AHB     peripheral bus
LPC81X_FMC:         .word    0x40040000 // Figure 6: memory mapping => AHB     peripheral bus

LPC81X_SYSTICK:     .word    0xE000E000 // Figure 6: memory mapping => private peripheral bus

#if defined( LPC81X_SYSTICK_56BIT )
                    .global  lpc81x_tsc

lpc81x_tsc:         .word    0xFFFFFFFF
#endif

// ----------------------------------------------------------------------------

                    .section .text.lpc81x, "x"

                    .thumb_func  
                    .global  lpc81x_irq_enable
           
lpc81x_irq_enable:  cpsie    I          // set CPSR[ I ] = 0

                    bx       lr         // return
     
                    .thumb_func  
                    .global  lpc81x_irq_unable

lpc81x_irq_unable:  cpsid    I          // set CPSR[ I ] = 1

                    bx       lr         // return

// ============================================================================

