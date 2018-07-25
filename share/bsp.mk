# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

vpath %.h .:../../share
vpath %.c .:../../share
vpath %.S .:../../share

BSP_DIRS     += ./build/lib
BSP_DIRS     += ./build/include/scale

BSP_INCLUDES += . 
BSP_INCLUDES += ../../share

BSP_HEADERS  += ./scale.h
BSP_HEADERS  += $(wildcard ../../share/bsp.h)
BSP_SOURCES  += ./scale.c  
BSP_SOURCES  += $(wildcard ../../share/bsp.c)

BSP_EXTRAS   += ./scale.conf
BSP_EXTRAS   += ./scale.ld
BSP_EXTRAS   += ./scale.mk

BSP_OBJECTS   = $(addprefix ./build/lib/,           $(notdir $(patsubst %.c, %.o, ${BSP_SOURCES})))

BSP_TARGETS   = $(addprefix ./build/include/scale/, $(notdir $(patsubst %.h, %.h, ${BSP_HEADERS})))
BSP_TARGETS  += ./build/lib/libscale.a
BSP_TARGETS  += ./build/lib/scale.conf
BSP_TARGETS  += ./build/lib/scale.ld
BSP_TARGETS  += ./build/lib/scale.mk
BSP_TARGETS  += ./build/lib/crt0.o

${BSP_DIRS} :
	@mkdir -p ${@}

./build/lib/%.o : %.c %.h
	@${GCC_PREFIX}gcc $(patsubst %, -I %, ${BSP_INCLUDES}) ${GCC_PATHS} ${GCC_FLAGS} -c -o ${@} ${<}
./build/lib/%.o : %.S
	@${GCC_PREFIX}gcc $(patsubst %, -I %, ${BSP_INCLUDES}) ${GCC_PATHS} ${GCC_FLAGS} -c -o ${@} ${<}

$(filter %.h,    ${BSP_TARGETS}) : ./build/include/%.h :                  ${BSP_HEADERS}
	@cp $(filter %$(notdir ${@}), ${^}) ${@}
$(filter %.a,    ${BSP_TARGETS}) : ./build/lib/%.a     :                  ${BSP_OBJECTS}
	@${GCC_PREFIX}ar rcs ${@} ${^}
$(filter %.conf, ${BSP_TARGETS}) : ./build/lib/%.conf  : $(filter %.conf, ${BSP_EXTRAS})
	@cp ${<} ${@} 
$(filter %.ld,   ${BSP_TARGETS}) : ./build/lib/%.ld    : $(filter %.ld,   ${BSP_EXTRAS})
	@cp ${<} ${@} 
$(filter %.mk,   ${BSP_TARGETS}) : ./build/lib/%.mk    : $(filter %.mk,   ${BSP_EXTRAS})
	@cp ${<} ${@} 

bsp-all   : ${BSP_DIRS} ${BSP_TARGETS}

bsp-clean :
	@rm -rf ./build/*
