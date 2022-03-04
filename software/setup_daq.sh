#!/usr/bin/env sh

export DAQ_INSTALL_PREFIX=/u1/ldmx/server/ldmx-daq/software/install

#Add usr/local/lib
export LD_LIBRARY_PATH=/usr/local/lib/:$LD_LIBRARY_PATH

#Add DAQ
export LD_LIBRARY_PATH=$DAQ_INSTALL_PREFIX/lib:$LD_LIBRARY_PATH
export PATH=$DAQ_INSTALL_PREFIX/bin:$PATH

#Add Rogue
export LD_LIBRARY_PATH=/u1/ldmx/server/rogue/lib/:$LD_LIBRARY_PATH
export PATH=/u1/ldmx/server/eudaq/bin:$PATH

#Add the fiber tracker
export LD_LIBRARY_PATH=/u1/ldmx/server/FiberTrackerDAQ/install/lib:$LD_LIBRARY_PATH

#Add dip
export LD_LIBRARY_PATH=/u1/ldmx/server/FiberTrackerDAQ/dip-5.7.0/lib64:$LD_LIBRARY_PATH

#Add WR library
export LD_LIBRARY_PATH=/u1/ldmx/server/WRTimestamping/build/:$LD_LIBRARY_PATH

