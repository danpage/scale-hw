# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

vpath %.h .:../../share:../../share/lpc13xx
vpath %.c .:../../share:../../share/lpc13xx
vpath %.s .:../../share:../../share/lpc13xx
vpath %.S .:../../share:../../share/lpc13xx

include scale.conf

BSP_INCLUDES =            ../../share/lpc13xx

BSP_HEADERS  = $(wildcard ../../share/lpc13xx/*.h ) 
BSP_SOURCES  = $(wildcard ../../share/lpc13xx/*.S ) 

BSP_SCRIPTS  = $(wildcard ../../share/lpc13xx/*.ld) 

include ../../share/bsp.mk
include ../../share/board.mk

all     : bsp

bsp     : bsp-all
board   :           board-all   

clean   : bsp-clean board-clean
