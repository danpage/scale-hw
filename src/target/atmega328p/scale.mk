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

GCC_FLAGS        += 
GCC_PATHS        += -I ${BSP}/include -L ${BSP}/lib
GCC_LIBS         += -lscale

PROJECT_INCLUDES += 

PROJECT_HEADERS  += ${PROJECT}.h
PROJECT_SOURCES  += ${PROJECT}.c

PROJECT_TARGETS  += ${PROJECT}.map
PROJECT_TARGETS  += ${PROJECT}.elf
PROJECT_TARGETS  += ${PROJECT}.bin
PROJECT_TARGETS  += ${PROJECT}.hex

%.elf %.map : ${PROJECT_SOURCES} ${PROJECT_HEADERS}
	@${GCC_PREFIX}gcc $(patsubst %, -I %, ${PROJECT_INCLUDES}) ${GCC_PATHS} ${GCC_FLAGS} ${SCALE_CONF} -Wl,-Map=${*}.map -o ${*}.elf ${PROJECT_SOURCES} ${GCC_LIBS}
%.bin       : %.elf
	@${GCC_PREFIX}objcopy ${OBJCOPY_FLAGS} --output-target="binary" ${<} ${@}
%.hex       : %.elf
	@${GCC_PREFIX}objcopy ${OBJCOPY_FLAGS} --output-target="ihex"   ${<} ${@}

include ${BSP}/share/putty.mk

all     : ${PROJECT_TARGETS}

clean   :
	@rm -f ${PROJECT_TARGETS}

program : ${PROJECT_TARGETS}
	@avrdude -v -e -V -P ${USB} -c arduino -p m328p -U flash:w:$(filter %.hex, ${^})

emulate : ${PROJECT_TARGETS}
	@python -O ${BSP}/share/emulator.py --file="$(filter %.hex, ${^})" --host="127.0.0.1" --port="1234" --target="atmega328p"

fuse    : 
	@avrdude -v -e -V -P ${USB} -c arduino -p m328p ${SCALE_FUSE}
