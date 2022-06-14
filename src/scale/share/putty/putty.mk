# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

# =============================================================================

${HOME}/.putty/sessions/scale-board    : ${BSP}/share/putty.scale-board
	@mkdir --parents $(dir ${@}) && cp ${BSP}/share/putty.scale-board    ${@}

${HOME}/.putty/sessions/scale-emulator : ${BSP}/share/putty.scale-emulator
	@mkdir --parents $(dir ${@}) && cp ${BSP}/share/putty.scale-emulator ${@}

# -----------------------------------------------------------------------------

putty-board    : ${HOME}/.putty/sessions/scale-board
	@putty -load $(notdir ${<})

putty-emulator : ${HOME}/.putty/sessions/scale-emulator
	@putty -load $(notdir ${<})

# =============================================================================
