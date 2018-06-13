/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

               .syntax  unified
               .thumb
      
               .extern    main
  
               .extern  __head_data
               .extern  __foot_data
               .extern  __load_data
               .extern  __head_bss
               .extern  __foot_bss
               .extern  __load_bss
               .extern  __tos
      
               .section .vector,  "x"
  
               .global  __vector
    
__vector:      .word    __tos           // #1  -> 0x00000000 Top of Stack    
               .word    __handler_rst   // #2  -> 0x00000004 reset   handler 
               .word    __handler_nul   // #3  -> 0x00000008 NMI     handler 
               .word    __handler_nul   // #4  -> 0x0000000C reserved        
               .word    __handler_nul   // #5  -> 0x00000010 reserved        
               .word    __handler_nul   // #6  -> 0x00000014 reserved        
               .word    __handler_nul   // #7  -> 0x00000018 reserved        
               .word    __handler_nul   // #8  -> 0x0000001C reserved        
               .word    __handler_nul   // #9  -> 0x00000020 reserved        
               .word    __handler_nul   // #10 -> 0x00000024 reserved        
               .word    __handler_nul   // #11 -> 0x00000028 SVC     handler 
               .word    __handler_nul   // #12 -> 0x0000002C reserved        
               .word    __handler_nul   // #13 -> 0x00000030 reserved        
               .word    __handler_nul   // #14 -> 0x00000034 PendSV  handler 
               .word    __handler_nul   // #15 -> 0x00000038 SysTick handler 
  
               .align   4
  
               .section .handler, "x"

               .thumb_func
               .global  __handler_nul

__handler_nul: b        .               // spin

               .align   4

               .thumb_func  
               .global  __handler_rst
	
__handler_rst: ldr      r1, __HEAD_DATA // r1 = >| .data
               ldr      r2, __FOOT_DATA // r2 =    .data |<
               ldr      r3, __LOAD_DATA // r3 =    .text |<
  
               subs     r2, r1          // r2 =  | .data |
               beq      l1              // if r2 = 0, goto l1

l0:            ldrb     r4, [ r3 ]      // r4 = MEM[ r3 ] => ROM[ i ]
               strb     r4, [ r1 ]      // MEM[ r1 ] = r4 => RAM[ i ]
               adds     r3, #1          // r3 += 1
               adds     r1, #1          // r1 += 1
               subs     r2, #1          // r2 -= 1
               bgt      l0              // if r2 > 0, goto l0

l1:            ldr      r1, __HEAD_BSS  // r1 = >| .bss
               ldr      r2, __FOOT_BSS  // r2 =    .bss |<
               subs     r2, r1          // r2 =  | .bss |
               beq      l3              // if r2 = 0, goto l3
  
               eors     r4, r4
l2:            strb     r4, [ r1 ]      // MEM[ r1 ] = r4 = 0
               adds     r1, #1          // r1 += 1
               subs     r2, #1          // r2 -= 1
               bgt      l2              // if r2 > 0, goto l2

l3:            bl       __start         // call __start
               b        .               // spin

               .align   4
	
__HEAD_DATA:   .word __head_data
__FOOT_DATA:   .word __foot_data
__LOAD_DATA:   .word __load_data
__HEAD_BSS:    .word __head_bss
__FOOT_BSS:    .word __foot_bss
__LOAD_BSS:    .word __load_bss

               .align   4
	
               .thumb_func  
               .global  __handler_rst
	
__start:       eors     r0, r0          // argc = 0
               eors     r1, r1          // argv = NULL
  
               bl       main            // call main
               mov      pc, lr          // return

               .align   4	
