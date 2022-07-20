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

# =============================================================================

include ${BSP}/lib/scale.conf

GCC_FLAGS        += -T ${BSP}/lib/scale.ld
GCC_PATHS        += -I ${BSP}/include -L ${BSP}/lib
GCC_LIBS         += -lscale

PROJECT_INCLUDES +=

PROJECT_HEADERS  += ${PROJECT}.h
PROJECT_SOURCES  += ${PROJECT}.c

PROJECT_TARGETS  += ${PROJECT}.map
PROJECT_TARGETS  += ${PROJECT}.elf
PROJECT_TARGETS  += ${PROJECT}.bin
PROJECT_TARGETS  += ${PROJECT}.hex

ifndef EMULATOR_HOST
EMULATOR_HOST     = 127.0.0.1
endif
ifndef EMULATOR_PORT
EMULATOR_PORT     = 1234
endif

# -----------------------------------------------------------------------------

include ${BSP}/share/putty.mk

%.elf %.map : ${PROJECT_SOURCES} ${PROJECT_HEADERS}
	@${GCC_PREFIX}gcc $(patsubst %, -I %, ${PROJECT_INCLUDES}) ${GCC_PATHS} ${GCC_FLAGS} ${SCALE_CONF} -Wl,-Map=${*}.map -o ${*}.elf ${PROJECT_SOURCES} ${GCC_LIBS}
%.bin       : %.elf
	@${GCC_PREFIX}objcopy ${OBJCOPY_FLAGS} --output-target="binary" ${<} ${@}
%.hex       : %.elf
	@${GCC_PREFIX}objcopy ${OBJCOPY_FLAGS} --output-target="ihex"   ${<} ${@}

# -----------------------------------------------------------------------------

build   :                           ${PROJECT_TARGETS}

clean   :
	@rm --force                 ${PROJECT_TARGETS}

program :           $(filter %.hex, ${PROJECT_TARGETS})
	@lpc21isp -wipe -hex ${<} ${USB} 9600 12000

emulate :           $(filter %.hex, ${PROJECT_TARGETS})
	@python3 -O ${BSP}/share/emulator.py --file="${<}" --host="${EMULATOR_HOST}" --port="${EMULATOR_PORT}" --target="lpc1313fbd48"

# =============================================================================
