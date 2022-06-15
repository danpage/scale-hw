# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

import argparse, binascii, logging, os, select, signal, socket, struct, sys

import  unicorn           as  unicorn
import  unicorn.arm_const as  unicorn_arm
import capstone           as capstone
import capstone.arm       as capstone_arm

import intelhex           as intelhex

def mmio_syscon_rd( emu, mode, addr, size, val, data ):
  if   ( addr == 0x4004800C ) : # SYSPLLSTAT
      emu.mem_write( 0x4004800C, bytes( [ 1 ] ) )
  elif ( addr == 0x40048044 ) : # SYSPLLCLKUEN
      emu.mem_write( 0x40048044, bytes( [ 1 ] ) )
  elif ( addr == 0x40048074 ) : # MAINCLKUEN
      emu.mem_write( 0x40048074, bytes( [ 1 ] ) )

def mmio_syscon_wr( emu, mode, addr, size, val, data ):
  pass

mmio_uart_server = None
mmio_uart_client = None

def mmio_uart_open() :
  global mmio_uart_server
  global mmio_uart_client

  mmio_uart_server = socket.socket( socket.AF_INET, socket.SOCK_STREAM ) ; mmio_uart_server.bind( ( args.host, args.port ) ) ; mmio_uart_server.listen( 1 ) ; ( mmio_uart_client, _ ) = mmio_uart_server.accept()

def mmio_uart_can_rd( x ) :
  ( t, _, _ ) = select.select( [ x ], [], [], 0.0 ) ; return x in t
def mmio_uart_can_wr( x ) :
  ( _, t, _ ) = select.select( [], [ x ], [], 0.0 ) ; return x in t

def mmio_uart_rd( emu, mode, addr, size, val, data ):
  if   ( addr == 0x40008000 ) : # U0RBR
    ( LCR, ) = struct.unpack( '<I', emu.mem_read( 0x4000800C, 4 ) )

    if ( ( LCR >> 7 ) & 1 ) :
      return

    if ( mmio_uart_can_rd( mmio_uart_client ) ) :
      emu.mem_write( 0x40008000, mmio_uart_client.recv( 1 ) )
    else :
      emu.mem_write( 0x40008000, struct.pack( '<I', 0x00000000 ) )

  elif ( addr == 0x4000800C ) : # U0LCR
    pass

  elif ( addr == 0x40008014 ) : # U0LSR
    if ( mmio_uart_can_rd( mmio_uart_client ) ) :
      emu.mem_write( 0x40008014, struct.pack( '<I', 0x00000021 ) ) # => no error, can   read, can write
    else :
      emu.mem_write( 0x40008014, struct.pack( '<I', 0x00000020 ) ) # => no error, can't read, can write

def mmio_uart_wr( emu, mode, addr, size, val, data ):
  if   ( addr == 0x40008000 ) : # U0THR
    ( LCR, ) = struct.unpack( '<I', emu.mem_read( 0x4000800C, 4 ) )

    if ( ( LCR >> 7 ) & 1 ) :
      return

    if ( mmio_uart_can_wr( mmio_uart_client ) ) :
      mmio_uart_client.send( bytes( [ val ] ) )
    else :
      pass

  elif ( addr == 0x4000800C ) : # U0LCR
    pass

  elif ( addr == 0x40008014 ) : # U0LSR
    pass

def fetch( emu, addr, size, data ):
  if ( args.debug ) :
    for instr in list( asm.disasm( emu.mem_read( addr, size ), size ) ) : 
      print( 'executing @ addr: {0:08X}, size: {1:02d} => instr: {2:s} {3:s}'.format( addr, size, instr.mnemonic, instr.op_str ) )

      print( 'R0  = {0:08X}  R1  = {1:08X}  R2  = {2:08X}  R3  = {3:08X}'.format( emu.reg_read( unicorn_arm.UC_ARM_REG_R0  ), emu.reg_read( unicorn_arm.UC_ARM_REG_R1  ), emu.reg_read( unicorn_arm.UC_ARM_REG_R2  ), emu.reg_read( unicorn_arm.UC_ARM_REG_R3  ) ) )
      print( 'R4  = {0:08X}  R5  = {1:08X}  R6  = {2:08X}  R7  = {3:08X}'.format( emu.reg_read( unicorn_arm.UC_ARM_REG_R4  ), emu.reg_read( unicorn_arm.UC_ARM_REG_R5  ), emu.reg_read( unicorn_arm.UC_ARM_REG_R6  ), emu.reg_read( unicorn_arm.UC_ARM_REG_R7  ) ) )
      print( 'R8  = {0:08X}  R9  = {1:08X}  R10 = {2:08X}  R11 = {3:08X}'.format( emu.reg_read( unicorn_arm.UC_ARM_REG_R8  ), emu.reg_read( unicorn_arm.UC_ARM_REG_R9  ), emu.reg_read( unicorn_arm.UC_ARM_REG_R10 ), emu.reg_read( unicorn_arm.UC_ARM_REG_R11 ) ) )
      print( 'R12 = {0:08X}  R13 = {1:08X}  R14 = {2:08X}  R15 = {3:08X}'.format( emu.reg_read( unicorn_arm.UC_ARM_REG_R12 ), emu.reg_read( unicorn_arm.UC_ARM_REG_R13 ), emu.reg_read( unicorn_arm.UC_ARM_REG_R14 ), emu.reg_read( unicorn_arm.UC_ARM_REG_R15 ) ) )
 
if ( __name__ == '__main__' ) :
  # parse command line arguments

  parser = argparse.ArgumentParser()

  parser.add_argument( '--target', dest = 'target',              action = 'store', choices = [ 'lpc1114fn28', 'lpc1313fbd48' ], default = 'lpc1313fbd48' )

  parser.add_argument( '--file',   dest = 'file',   type =  str, action = 'store' )
  parser.add_argument( '--host',   dest = 'host',   type =  str, action = 'store' )
  parser.add_argument( '--port',   dest = 'port',   type =  int, action = 'store' )

  parser.add_argument( '--debug',  dest = 'debug',               action = 'store_true' )

  args = parser.parse_args()

  if ( args.debug ) :
    l = logging.DEBUG
  else :
    l = logging.INFO

  logging.basicConfig( stream = sys.stdout, level = l, format = '%(filename)s : %(asctime)s : %(message)s', datefmt = '%d/%m/%y @ %H:%M:%S' )

  # create emulator and disassembler

  emu =  unicorn.Uc(  unicorn.UC_ARCH_ARM,  unicorn.UC_MODE_THUMB )
  asm = capstone.Cs( capstone.CS_ARCH_ARM, capstone.CS_MODE_THUMB )
  
  if   ( args.target == 'lpc1114fn28'  ) :
    emu.mem_map( 0x00000000,  32 * 1024        ) # Figure  6:  32 kB => flash 
    emu.mem_map( 0x10000000,   4 * 1024        ) # Figure  6:   4 kB => SRAM
    emu.mem_map( 0x1FFF0000,  16 * 1024        ) # Figure  6:  16 kB => boot ROM
    emu.mem_map( 0x40000000, 512 * 1024        ) # Figure  6: 512 kB => APB     peripheral bus
    emu.mem_map( 0x50000000,   2 * 1024 * 1024 ) # Figure  6:   2 MB => AHB     peripheral bus
    emu.mem_map( 0xE0000000,   1 * 1024 * 1024 ) # Figure  6:   1 MB => private peripheral bus

  elif ( args.target == 'lpc1313fbd48' ) :
    emu.mem_map( 0x00000000,  32 * 1024        ) # Figure 14:  32 kB => flash 
    emu.mem_map( 0x10000000,   8 * 1024        ) # Figure 14:   8 kB => SRAM
    emu.mem_map( 0x1FFF0000,  16 * 1024        ) # Figure 14:  16 kB => boot ROM
    emu.mem_map( 0x40000000, 512 * 1024        ) # Figure  6: 512 kB => APB     peripheral bus
    emu.mem_map( 0x50000000,   2 * 1024 * 1024 ) # Figure  6:   2 MB => AHB     peripheral bus
    emu.mem_map( 0xE0000000,   1 * 1024 * 1024 ) # Figure  6:   1 MB => private peripheral bus

  # hook instruction fetch, and handle Ctrl-c as "forced exit"

  emu.hook_add( unicorn.UC_HOOK_CODE, fetch )

  def handler( s, f ) :
    exit( 0 )

  signal.signal( signal.SIGINT, handler )

  # hook access to memory mapped SYSCON

  emu.hook_add( unicorn.UC_HOOK_MEM_READ,  mmio_syscon_rd, begin = 0x40048000, end = 0x4004C000 - 1 )
  emu.hook_add( unicorn.UC_HOOK_MEM_WRITE, mmio_syscon_wr, begin = 0x40048000, end = 0x4004C000 - 1 )

  # book access to memory mapped UART

  emu.hook_add( unicorn.UC_HOOK_MEM_READ,  mmio_uart_rd,   begin = 0x40008000, end = 0x4000C000 - 1 )
  emu.hook_add( unicorn.UC_HOOK_MEM_WRITE, mmio_uart_wr,   begin = 0x40008000, end = 0x4000C000 - 1 )

  mmio_uart_open()

  # program emulator

  img = intelhex.IntelHex( args.file )
  
  for addr in img.addresses() :
    emu.mem_write( addr, bytes( [ img[ addr ] ] ) )
  
  # reset   emulator

  ( tos, ) = struct.unpack( '<I', emu.mem_read( 0x00000000, 4 ) )
  ( rst, ) = struct.unpack( '<I', emu.mem_read( 0x00000004, 4 ) )

  emu.reg_write( unicorn_arm.UC_ARM_REG_R0,  0   )
  emu.reg_write( unicorn_arm.UC_ARM_REG_R1,  0   )
  emu.reg_write( unicorn_arm.UC_ARM_REG_R2,  0   )
  emu.reg_write( unicorn_arm.UC_ARM_REG_R3,  0   )
  emu.reg_write( unicorn_arm.UC_ARM_REG_R4,  0   )
  emu.reg_write( unicorn_arm.UC_ARM_REG_R5,  0   )
  emu.reg_write( unicorn_arm.UC_ARM_REG_R6,  0   )
  emu.reg_write( unicorn_arm.UC_ARM_REG_R7,  0   )
  emu.reg_write( unicorn_arm.UC_ARM_REG_R8,  0   )
  emu.reg_write( unicorn_arm.UC_ARM_REG_R9,  0   )
  emu.reg_write( unicorn_arm.UC_ARM_REG_R10, 0   )
  emu.reg_write( unicorn_arm.UC_ARM_REG_R11, 0   )
  emu.reg_write( unicorn_arm.UC_ARM_REG_R12, 0   )
  emu.reg_write( unicorn_arm.UC_ARM_REG_R13, 0   )
  emu.reg_write( unicorn_arm.UC_ARM_REG_R14, 0   )
  emu.reg_write( unicorn_arm.UC_ARM_REG_R15, 0   )
  
  emu.reg_write( unicorn_arm.UC_ARM_REG_SP,  tos )

  # start   emulator

  emu.emu_start( rst, ( 2 ** 32 ) - 1 )  

