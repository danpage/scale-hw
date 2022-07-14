# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

# =============================================================================

${HOME}/.putty/sessions/scale-physical : ${BSP}/share/putty.scale-physical
	@mkdir --parents $(dir ${@}) && cp ${BSP}/share/putty.scale-physical ${@}

${HOME}/.putty/sessions/scale-emulated : ${BSP}/share/putty.scale-emulated
	@mkdir --parents $(dir ${@}) && cp ${BSP}/share/putty.scale-emulated ${@}

# -----------------------------------------------------------------------------

putty-physical : ${HOME}/.putty/sessions/scale-physical
	@putty -load $(notdir ${<})

putty-emulated : ${HOME}/.putty/sessions/scale-emulated
	@putty -load $(notdir ${<})

# =============================================================================
