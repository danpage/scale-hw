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
                    .syntax  unified
                    .thumb

// ============================================================================

                    .section .data.lpc111x, "w"

                    .global  LPC111X_SYSCON
                    .global  LPC111X_IOCON
                    .global  LPC111X_GPIO0
                    .global  LPC111X_GPIO1
                    .global  LPC111X_GPIO2
                    .global  LPC111X_GPIO3
                    .global  LPC111X_UART
                    .global  LPC111X_I2C
                    .global  LPC111X_FMC

                    .global  LPC111X_SYSTICK
	
LPC111X_SYSCON:     .word    0x40048000 // Figure 6: memory mapping => AHB     peripheral bus
LPC111X_IOCON:      .word    0x40044000 // Figure 6: memory mapping => AHB     peripheral bus
LPC111X_GPIO0:      .word    0x50000000 // Figure 6: memory mapping => AHB     peripheral bus, GPIO0 => PIO0
LPC111X_GPIO1:      .word    0x50010000 // Figure 6: memory mapping => AHB     peripheral bus, GPIO1 => PIO1
LPC111X_GPIO2:      .word    0x50020000 // Figure 6: memory mapping => AHB     peripheral bus, GPIO2 => PIO2
LPC111X_GPIO3:      .word    0x50030000 // Figure 6: memory mapping => AHB     peripheral bus, GPIO3 => PIO3
LPC111X_UART:       .word    0x40008000 // Figure 6: memory mapping => AHB     peripheral bus
LPC111X_I2C:        .word    0x40000000 // Figure 6: memory mapping => AHB     peripheral bus
LPC111X_FMC:        .word    0x4003C000 // Figure 6: memory mapping => AHB     peripheral bus

LPC111X_SYSTICK:    .word    0xE000E000 // Figure 6: memory mapping => private peripheral bus

#if defined( LPC111X_SYSTICK_56BIT )
                    .global  lpc111x_tsc

lpc111x_tsc:        .word    0xFFFFFFFF
#endif

// ----------------------------------------------------------------------------

                    .section .text.lpc111x, "x"

                    .thumb_func  
                    .global  lpc111x_irq_enable
           
lpc111x_irq_enable: cpsie    I          // set CPSR[ I ] = 0

                    bx       lr         // return
     
                    .thumb_func  
                    .global  lpc111x_irq_unable

lpc111x_irq_unable: cpsid    I          // set CPSR[ I ] = 1

                    bx       lr         // return

// ============================================================================	

