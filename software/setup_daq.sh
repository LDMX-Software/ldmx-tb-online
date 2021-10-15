#!/usr/bin/env sh

export DAQ_INSTALL_PREFIX=/u1/ldmx/server/ldmx-daq/software/install
export LD_LIBRARY_PATH=$DAQ_INSTALL_PREFIX/lib:$LD_LIBRARY_PATH
export PATH=$DAQ_INSTALL_PREFIX/bin:$PATH

export LD_LIBRARY_PATH=/u1/ldmx/server/anaconda3/envs/rogue_main/lib/:$LD_LIBRARY_PATH
export PATH=/u1/ldmx/server/anaconda3/envs/rogue_main/bin/:$PATH

export PATH=/u1/ldmx/server/eudaq/bin:$PATH

source /u1/ldmx/server/root/bin/thisroot.sh

alias sdk='source /u1/ldmx/server/V3.4.1/i86-linux-64/tools/envs-sdk.sh'
