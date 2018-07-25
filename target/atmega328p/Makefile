# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

include scale.conf

BSP_HEADERS   = ../../share/atmega328p.h
BSP_SOURCES   = ../../share/atmega328p.c

GCC_PREFIX    = avr-
GCC_FLAGS     = ${SCALE_CONF} -Wall -std=gnu99 -Os -mmcu=atmega328p

include ../../share/bsp.mk
include ../../share/board.mk

all     : bsp

bsp     : bsp-all
board   :           board-all   

clean   : bsp-clean board-clean
