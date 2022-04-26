# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

vpath %.h .:../../share:../../share/lpc111x
vpath %.c .:../../share:../../share/lpc111x
vpath %.s .:../../share:../../share/lpc111x
vpath %.S .:../../share:../../share/lpc111x

include scale.conf

BSP_INCLUDES      =            ../../share/lpc111x

BSP_HEADERS       = $(wildcard ../../share/lpc111x/*.h )
BSP_SOURCES       = $(wildcard ../../share/lpc111x/*.S )

BSP_RANDOMS_LIB   = $(wildcard ../../share/lpc111x/*.ld)
BSP_RANDOMS_SHARE = 

include ../../share/bsp.mk
include ../../share/board.mk

all     : bsp

bsp     : bsp-all
board   :           board-all   

clean   : bsp-clean board-clean
