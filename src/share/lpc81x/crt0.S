/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */
                    .syntax  unified
                    .thumb

#if defined( LPC81X_SYSTICK_56BIT )
                    .extern    lpc81x_tsc
#endif	
	
                    .extern    main
  
                    .extern  __head_data
                    .extern  __foot_data
                    .extern  __load_data

                    .extern  __head_bss
                    .extern  __foot_bss
                    .extern  __load_bss

                    .extern  __tos

// ============================================================================

/* Interrupt (or exception) vector table, per Figure 2.2 of
 *
 * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0552a/BABIFJFG.html
 */
                    .section .data.__irq_handler, "w"

                    .global  __irq_table

__irq_table:        .word    __tos             // 0x00000000 => Top of Stack    
                    .word    __irq_handler_rst // 0x00000004 => reset         handler 
                    .word    __irq_handler_nul // 0x00000008 => NMI           handler 
                    .word    __irq_handler_nul // 0x0000000C => Hard  fault   handler
                    .word    __irq_handler_nul // 0x00000010 => MMU   fault   handler
                    .word    __irq_handler_nul // 0x00000014 => Bus   fault   handler
                    .word    __irq_handler_nul // 0x00000018 => Usage fault   Handler
                    .word    __irq_handler_nul // 0x0000001C => reserved        
                    .word    __irq_handler_nul // 0x00000020 => reserved        
                    .word    __irq_handler_nul // 0x00000024 => reserved        
                    .word    __irq_handler_nul // 0x00000028 => reserved        
                    .word    __irq_handler_nul // 0x0000002C => SVC           handler 
                    .word    __irq_handler_nul // 0x00000030 => Debug monitor handler
                    .word    __irq_handler_nul // 0x00000034 => reserved        
                    .word    __irq_handler_nul // 0x00000038 => PendSV        handler 
#if defined( LPC81X_SYSTICK_56BIT )
                    .word    __irq_handler_tsc // 0x0000003C => SysTick       handler 
#else
                    .word    __irq_handler_nul // 0x0000003C => SysTick       handler 
#endif
  
// ----------------------------------------------------------------------------

                    .section .text.__irq_handler, "x"

                    .global  __irq_handler_nul
                    .thumb_func

__irq_handler_nul:  b        .                 // spin ~= halt

                    .global  __irq_handler_rst
                    .thumb_func  

__irq_handler_rst:  ldr      r0, __HEAD_DATA   // r0  = >| .data
                    ldr      r1, __FOOT_DATA   // r1  =    .data |<
                    ldr      r2, __LOAD_DATA   // r2  =    .text |<
  
                    subs     r1, r0            // r1  =  | .data |
                    beq      __l1              // if r1 = 0, goto __l1

__l0:               ldrb     r3, [ r2 ]        // r3 = MEM[ r2 ] => ROM[ i ]
                    strb     r3, [ r0 ]        // MEM[ r0 ] = r3 => RAM[ i ]
                    adds     r2, #1            // r2 += 1
                    adds     r0, #1            // r0 += 1
                    subs     r1, #1            // r1 -= 1
                    bgt      __l0              // if r1 > 0, goto __l0

__l1:               ldr      r0, __HEAD_BSS    // r0  = >| .bss
                    ldr      r1, __FOOT_BSS    // r1  =    .bss |<
                    subs     r1, r0            // r1  =  | .bss |
                    beq      __l3              // if r1 = 0, goto __l3
  
                    eors     r3, r3
__l2:               strb     r3, [ r0 ]        // MEM[ r0 ] = r3 = 0
                    adds     r0, #1            // r0 += 1
                    subs     r1, #1            // r1 -= 1
                    bgt      __l2              // if r1 > 0, goto __l2

__l3:               bl       __start           // call __start

                    b        .                 // spin ~= halt

__HEAD_DATA:        .word    __head_data
__FOOT_DATA:        .word    __foot_data
__LOAD_DATA:        .word    __load_data

__HEAD_BSS:         .word    __head_bss
__FOOT_BSS:         .word    __foot_bss
__LOAD_BSS:         .word    __load_bss

#if defined( LPC81X_SYSTICK_56BIT )
                    .global  __handler_tsc
                    .thumb_func  

__irq_handler_tsc:  ldr      r0, =lpc81x_tsc  // r0  = &lpc81x_tsc

                    ldr      r1, [ r0 ]        // r1  = MEM[ r0 ] = lpc81x_tsc
	            subs     r1, #1            // r1 += 1
	            str      r1, [ r0 ]        // MEM[ r0 ]  = r1 = lpc81x_tsc + 1

                    bx       lr                // return
#endif

// ============================================================================

                    .section .text.__start,       "x"	

                    .global  __start	
                    .thumb_func  
	
__start:            eors     r0, r0            // argc  = 0
                    eors     r1, r1            // argv  = NULL
  
                    bl         main            // call    main

                    b        .                 // spin ~= halt

// ============================================================================
