# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

BOARD_SOURCES += board.sch
BOARD_SOURCES += board.brd
BOARD_SOURCES += board.lbr
BOARD_SOURCES += board.csv

BOARD_TARGETS  = board.pdf

${BOARD_TARGETS} : %.pdf : ${BOARD_SOURCES}
	@${EAGLEDIR}/bin/eagle -C 'edit $(filter %.sch, ${^}); set confirm yes; print landscape 10.0 -1 -caption file ${@} sheets all paper a3; quit;' $(filter %.sch, ${^})

board-all   : ${BOARD_TARGETS}

board-clean :
	@rm -f board.s#[1-9] board.s##
	@rm -f board.b#[1-9] board.b##
	@rm -f board.l#[1-9] board.l##
