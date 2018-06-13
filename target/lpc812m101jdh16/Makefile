# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

include scale.conf

BSP_HEADERS   = ../../share/lpc81x.h
BSP_SOURCES   = ../../share/lpc81x.c

GCC_PREFIX    = arm-none-eabi
GCC_FLAGS     = ${SCALE_CONF} -Wall -std=gnu99 -Os -mcpu=cortex-m0plus -mthumb

include ../../share/bsp.mk
include ../../share/board.mk

all     : bsp

bsp     : bsp-all
board   :           board-all   

clean   : bsp-clean board-clean
