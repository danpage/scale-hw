# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

ifndef PROJECT
  $(warning PROJECT environment variable not set)
endif
ifndef BSP
  $(warning     BSP environment variable not set)
endif
ifndef USB
  $(warning     USB environment variable not set)
endif

include ${BSP}/lib/scale.conf

INCLUDES    =

HEADERS     = ${PROJECT}.h
SOURCES     = ${PROJECT}.c

TARGETS     = ${PROJECT}.map
TARGETS    += ${PROJECT}.elf
TARGETS    += ${PROJECT}.bin
TARGETS    += ${PROJECT}.hex

GCC_PREFIX  = arm-none-eabi-
GCC_FLAGS   = ${SCALE_CONF} -Wall -std=gnu99 -Os -mcpu=cortex-m0plus -mthumb -nostartfiles -T ${BSP}/lib/scale.ld
GCC_PATHS   = -I ${BSP}/include -L ${BSP}/lib 
GCC_LIBS    = -lscale --specs=nano.specs

%.elf %.map : ${SOURCES} ${HEADERS}
	@${GCC_PREFIX}-gcc $(patsubst %, -I %, ${INCLUDES}) ${GCC_PATHS} ${GCC_FLAGS} -Wl,-Map=${*}.map -o ${*}.elf ${BSP}/lib/crt0.o ${SOURCES} ${GCC_LIBS}
%.bin       : %.elf
	@${GCC_PREFIX}-objcopy --gap-fill=0 -O binary ${<} ${@}
%.hex       : %.elf
	@${GCC_PREFIX}-objcopy --gap-fill=0 -O ihex   ${<} ${@}

all     : ${TARGETS}

clean   : 
	@rm -f ${TARGETS}

program : ${TARGETS}
	@lpc21isp -wipe -hex $(filter %.hex, ${^}) ${USB} 9600 12000
