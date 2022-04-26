# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

BSP_DIRS          += ./build/include/scale
BSP_DIRS          += ./build/lib 
BSP_DIRS          += ./build/share

BSP_INCLUDES      += ./        
BSP_INCLUDES      += ../../share

BSP_HEADERS       += ./scale.h 
BSP_HEADERS       += ../../share/bsp.h
BSP_SOURCES       += ./scale.c 
BSP_SOURCES       += ../../share/bsp.c

BSP_RANDOMS_LIB   += ./scale.conf
BSP_RANDOMS_LIB   += ./scale.ld
BSP_RANDOMS_LIB   += ./scale.mk
BSP_RANDOMS_SHARE +=  $(wildcard ../../share/putty/*) ../../emulator/emulator.py

BSP_OBJECTS       += $(addprefix ./build/lib/,           $(notdir $(patsubst %.c, %.o, $(filter %.c, ${BSP_SOURCES}))    ))
BSP_OBJECTS       += $(addprefix ./build/lib/,           $(notdir $(patsubst %.s, %.o, $(filter %.s, ${BSP_SOURCES}))    ))
BSP_OBJECTS       += $(addprefix ./build/lib/,           $(notdir $(patsubst %.S, %.o, $(filter %.S, ${BSP_SOURCES}))    ))

BSP_TARGETS       += $(addprefix ./build/include/scale/, $(notdir                                    ${BSP_HEADERS}      ))
BSP_TARGETS       += $(addprefix ./build/lib/,           $(notdir                                    ${BSP_RANDOMS_LIB}  )) ./build/lib/libscale.a
BSP_TARGETS       += $(addprefix ./build/share/,         $(notdir                                    ${BSP_RANDOMS_SHARE}))

.PRECIOUS: ${BSP_OBJECTS}

${BSP_DIRS} :
	@mkdir --parents ${@}

./build/lib/%.o : %.c %.h
	@${GCC_PREFIX}gcc $(patsubst %, -I %, ${BSP_INCLUDES}) ${GCC_PATHS} ${GCC_FLAGS} ${SCALE_CONF} -c -o ${@} ${<}
./build/lib/%.o : %.s %.h
	@${GCC_PREFIX}gcc $(patsubst %, -I %, ${BSP_INCLUDES}) ${GCC_PATHS} ${GCC_FLAGS} ${SCALE_CONF} -c -o ${@} ${<}
./build/lib/%.o : %.S %.h
	@${GCC_PREFIX}gcc $(patsubst %, -I %, ${BSP_INCLUDES}) ${GCC_PATHS} ${GCC_FLAGS} ${SCALE_CONF} -c -o ${@} ${<}

./build/lib/%.a : ${BSP_OBJECTS}
	@${GCC_PREFIX}ar rcs ${@} ${^}

define cp
./build/$(strip ${1})/$(notdir $(strip ${2})) : ${2}
	@cp $${<} $${@} 
endef

$(foreach FILE, ${BSP_HEADERS},       $(eval $(call cp, include/scale, ${FILE})))
$(foreach FILE, ${BSP_RANDOMS_LIB},   $(eval $(call cp, lib,           ${FILE})))
$(foreach FILE, ${BSP_RANDOMS_SHARE}, $(eval $(call cp, share,         ${FILE})))

bsp-all   : ${BSP_DIRS} ${BSP_TARGETS}

bsp-clean :
	@rm -rf ./build/*
