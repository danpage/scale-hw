# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

vpath %.h .:../../share:../../share/lpc81x
vpath %.c .:../../share:../../share/lpc81x
vpath %.s .:../../share:../../share/lpc81x
vpath %.S .:../../share:../../share/lpc81x

include scale.conf

BSP_INCLUDES =            ../../share/lpc81x

BSP_HEADERS  = $(wildcard ../../share/lpc81x/*.h )
BSP_SOURCES  = $(wildcard ../../share/lpc81x/*.S )

BSP_SCRIPTS  = $(wildcard ../../share/lpc81x/*.ld)

include ../../share/bsp.mk
include ../../share/board.mk

all     : bsp

bsp     : bsp-all
board   :           board-all   

clean   : bsp-clean board-clean
