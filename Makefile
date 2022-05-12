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

venv     : ${REPO_HOME}/requirements.txt
	@bash ${REPO_HOME}/bin/venv.sh

clean    :

spotless : clean
	@rm --force --recursive ${REPO_HOME}/build/*

# ============================================================================= 
