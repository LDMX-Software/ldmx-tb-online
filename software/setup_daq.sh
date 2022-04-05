#!/usr/bin/env sh

export DAQ_INSTALL_PREFIX=/u1/ldmx/server/ldmx-tb-online/software/install

#Add usr/local/lib
export LD_LIBRARY_PATH=/usr/local/lib:/usr/local/lib64:/opt/cactus/lib:$LD_LIBRARY_PATH

#Add DAQ
export LD_LIBRARY_PATH=$DAQ_INSTALL_PREFIX/lib:$LD_LIBRARY_PATH
export PATH=$DAQ_INSTALL_PREFIX/bin:$PATH

#Add Rogue
source /u1/ldmx/server/rogue/setup_rogue.sh
export LD_LIBRARY_PATH=/u1/ldmx/server/rogue/lib/:$LD_LIBRARY_PATH
export PATH=/u1/ldmx/server/eudaq/bin:$PATH

#Add the fiber tracker
export FiberTrackerDAQ_DIR=/u1/ldmx/server/FiberTrackerDAQ/install
export LD_LIBRARY_PATH=${FiberTrackerDAQ_DIR}/lib:${dip_DIR}/lib:$LD_LIBRARY_PATH

#Add dip
export dip_DIR=/u1/ldmx/server/FiberTrackerDAQ/dip-5.7.0/
export LD_LIBRARY_PATH=/u1/ldmx/server/FiberTrackerDAQ/dip-5.7.0/lib64:$LD_LIBRARY_PATH

#Add WR library
export WRTiming_DIR=/u1/ldmx/server/WRTimestamping
export LD_LIBRARY_PATH=${WRTiming_DIR}/lib:$LD_LIBRARY_PATH

# pflib
export pflib_DIR=/u1/ldmx/server/pflib/install
export LD_LIBRARY_PATH=${pflib_DIR}/lib:${LD_LIBRARY_PATH}
export PATH=${pflib_DIR}/bin:${PATH}

# add root
source /u1/ldmx/server/root/root_install/bin/thisroot.sh
