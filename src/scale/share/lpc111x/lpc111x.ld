/* Copyright (C) 2017 Daniel Page <dan@phoo.org>
 *
 * Use of this source code is restricted per the CC BY-SA license, a copy of
 * which can be found via http://creativecommons.org (and should be included 
 * as LICENSE.txt within the associated archive or repository).
 */

ENTRY( __irq_table )

SECTIONS {
  . = 0x00000000;

  .text         : ALIGN( 16 ) {
    __head_text = .;
    *( .data.__irq_handler )
    *( .text.__irq_handler )
    *( .text*              )
    *( .text.*             )
    *( .rodata*            )
    *( .rodata.*           )
    . = ALIGN( 4 );
    __foot_text = .;
  } >         rom

  .data         : ALIGN( 16 ) {
    __head_data = .;
    *( .data               )
    *( .data.*             )
    . = ALIGN( 4 );
    __foot_data = .;
  } > ram AT> rom

  .bss (NOLOAD) : ALIGN( 16 ) {
    __head_bss  = .;
    *( .bss                )
    *( .bss.*              )
    . = ALIGN( 4 );
    __foot_bss  = .;
  } > ram
}

PROVIDE( __load_text = LOADADDR( .text ) );
PROVIDE( __load_data = LOADADDR( .data ) );
PROVIDE( __load_bss  = LOADADDR( .bss  ) );

PROVIDE( __tos = ORIGIN( ram ) + LENGTH( ram ) );
