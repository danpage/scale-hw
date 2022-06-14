# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

ifndef REPO_HOME
  $(error "execute 'source ./bin/conf.sh' to configure environment")
endif
ifndef REPO_VERSION
  $(error "execute 'source ./bin/conf.sh' to configure environment")
endif

# =============================================================================

BSP_BUILD          = ${REPO_HOME}/build/${TARGET}

BSP_DIRS          += ${BSP_BUILD}/include/scale
BSP_DIRS          += ${BSP_BUILD}/lib 
BSP_DIRS          += ${BSP_BUILD}/share

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

BSP_OBJECTS       += $(addprefix ${BSP_BUILD}/lib/,           $(notdir $(patsubst %.c, %.o, $(filter %.c, ${BSP_SOURCES}))    ))
BSP_OBJECTS       += $(addprefix ${BSP_BUILD}/lib/,           $(notdir $(patsubst %.s, %.o, $(filter %.s, ${BSP_SOURCES}))    ))
BSP_OBJECTS       += $(addprefix ${BSP_BUILD}/lib/,           $(notdir $(patsubst %.S, %.o, $(filter %.S, ${BSP_SOURCES}))    ))

BSP_TARGETS       += $(addprefix ${BSP_BUILD}/include/scale/, $(notdir                                    ${BSP_HEADERS}      ))
BSP_TARGETS       += $(addprefix ${BSP_BUILD}/lib/,           $(notdir                                    ${BSP_RANDOMS_LIB}  )) ${BSP_BUILD}/lib/libscale.a
BSP_TARGETS       += $(addprefix ${BSP_BUILD}/share/,         $(notdir                                    ${BSP_RANDOMS_SHARE}))

# -----------------------------------------------------------------------------

define cp
${BSP_BUILD}/$(strip ${1})/$(notdir $(strip ${2})) : ${2}
	@cp $${<} $${@} 
endef

# -----------------------------------------------------------------------------

.PRECIOUS: ${BSP_OBJECTS}

${BSP_DIRS} :
	@mkdir --parents ${@}

${BSP_BUILD}/lib/%.o : %.c %.h
	@${GCC_PREFIX}gcc $(patsubst %, -I %, ${BSP_INCLUDES}) ${GCC_PATHS} ${GCC_FLAGS} ${SCALE_CONF} -c -o ${@} ${<}
${BSP_BUILD}/lib/%.o : %.s %.h
	@${GCC_PREFIX}gcc $(patsubst %, -I %, ${BSP_INCLUDES}) ${GCC_PATHS} ${GCC_FLAGS} ${SCALE_CONF} -c -o ${@} ${<}
${BSP_BUILD}/lib/%.o : %.S %.h
	@${GCC_PREFIX}gcc $(patsubst %, -I %, ${BSP_INCLUDES}) ${GCC_PATHS} ${GCC_FLAGS} ${SCALE_CONF} -c -o ${@} ${<}

${BSP_BUILD}/lib/%.a : ${BSP_OBJECTS}
	@${GCC_PREFIX}ar rcs ${@} ${^}

$(foreach FILE, ${BSP_HEADERS},       $(eval $(call cp, include/scale, ${FILE})))
$(foreach FILE, ${BSP_RANDOMS_LIB},   $(eval $(call cp, lib,           ${FILE})))
$(foreach FILE, ${BSP_RANDOMS_SHARE}, $(eval $(call cp, share,         ${FILE})))

# -----------------------------------------------------------------------------

bsp-build : ${BSP_DIRS} ${BSP_TARGETS}

bsp-clean :
	@rm --force --recursive ${BSP_BUILD}

# =============================================================================
