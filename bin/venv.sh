#!/bin/bash

# Copyright (C) 2017 Daniel Page <dan@phoo.org>
#
# Use of this source code is restricted per the CC BY-SA license, a copy of
# which can be found via http://creativecommons.org (and should be included 
# as LICENSE.txt within the associated archive or repository).

if [ -z ${REPO_HOME} ] ; then
  echo "REPO_HOME environment variable undefined: aborting" ; exit
fi

python3 -m venv --clear ${REPO_HOME}/build/venv

source ${REPO_HOME}/build/venv/bin/activate

python3 -m pip install --upgrade pip
python3 -m pip install -r ${REPO_HOME}/requirements.txt
