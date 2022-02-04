#!/usr/bin/env sh

export DAQ_INSTALL_PREFIX=/u1/ldmx/server/ldmx-daq/software/install
export LD_LIBRARY_PATH=$DAQ_INSTALL_PREFIX/lib:$LD_LIBRARY_PATH
export PATH=$DAQ_INSTALL_PREFIX/bin:$PATH

export LD_LIBRARY_PATH=/u1/ldmx/server/rogue/lib/:$LD_LIBRARY_PATH
export PATH=/u1/ldmx/server/eudaq/bin:$PATH

#source /u1/ldmx/server/root/bin/thisroot.sh
export LD_LIBRARY_PATH=/u1/ldmx/server/FiberTrackerDAQ/install/lib:$LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/u1/ldmx/server/FiberTrackerDAQ/dip-5.7.0/lib64:$LD_LIBRARY_PATH

