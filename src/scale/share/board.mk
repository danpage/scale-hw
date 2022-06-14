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

BOARD_BUILD    = ${REPO_HOME}/build/${TARGET}

BOARD_DIRS     = ${BOARD_BUILD}/doc

BOARD_SOURCES +=                board.sch
BOARD_SOURCES +=                board.brd
BOARD_SOURCES +=                board.lbr
BOARD_SOURCES +=                board.csv

BOARD_TARGETS  = ${BOARD_BUILD}/board.pdf

# -----------------------------------------------------------------------------

${BOARD_DIRS} :
	@mkdir --parents ${@}

${BOARD_TARGETS} : %.pdf : ${BOARD_SOURCES}
	@${EAGLEDIR}/bin/eagle -C 'edit $(filter %.sch, ${^}); set confirm yes; print landscape 10.0 -1 -caption file ${@} sheets all paper a3; quit;' $(filter %.sch, ${^})

# -----------------------------------------------------------------------------

board-build : ${BOARD_DIRS} ${BOARD_TARGETS}

board-clean :
	@rm --force --recursive ${BOARD_BUILD}

	@rm --force board.s#[1-9] board.s##
	@rm --force board.b#[1-9] board.b##
	@rm --force board.l#[1-9] board.l##

# =============================================================================
