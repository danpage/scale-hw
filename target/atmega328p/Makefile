# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

vpath %.h .:../../share:../../share/atmega328p
vpath %.c .:../../share:../../share/atmega328p
vpath %.s .:../../share:../../share/atmega328p
vpath %.S .:../../share:../../share/atmega328p

include scale.conf

BSP_INCLUDES =            ../../share/atmega328p

BSP_HEADERS  = $(wildcard ../../share/atmega328p/*.h )
BSP_SOURCES  = $(wildcard ../../share/atmega328p/*.S )

BSP_SCRIPTS  = $(wildcard ../../share/atmega328p/*.ld)

include ../../share/bsp.mk
include ../../share/board.mk

all     : bsp

bsp     : bsp-all
board   :           board-all   

clean   : bsp-clean board-clean
