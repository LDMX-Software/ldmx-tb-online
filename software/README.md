# ldmx-tb-online
Test beam run control software used by the LDMX experiment. 

## Required Dependencies

### Rogue ([v5.13.0](https://slaclab.github.io/rogue/index.html))
#### Installation
The instructions below outline how to install `rogue` from source (the preferred method for the test beam).

To begin, clone the project and checkout the `v5.13.0` tag as follows
```bash
git clone git@github.com:slaclab/rogue.git
cd rogue
git checkout tags/v5.13.0 -b v5.13.0
```
Once all [dependencies](https://slaclab.github.io/rogue/installing/build.html#installing-packages-required-for-rogue) have been installed, `rogue` is built by issuing the following command
```bash
pip3 install -r pip_requirements.txt
mkdir build
cd build
cmake .. -DROGUE_INSTALL=local
make
make install
```
This will build and install everything in the root rogue directory.  The installation 
will also create a script in the `rogue` root directory that can be sourced 
to add `rogue` to the environment
```bash
source setup_rogue.sh
```

### eudaq ([Docs](https://eudaq.github.io/))
#### Installation
Installation of `eudaq`, requires the dependencies outline in the [README](https://github.com/eudaq/eudaq/blob/master/README.md#for-the-core-library-executables-and-gui). 

  ℹ️ `ROOT` is required for the monitoring app. If you are able to install ROOT, install it _before_ eudaq so that you can activate the ROOT-based monitoring within eudaq.

Once 
the depdendencies have been installed, `eudaq` can be built as follows
```bash
git clone https://github.com/eudaq/eudaq.git eudaq 
mkdir build
cd build
cmake -DEUDAQ_BUILD_ONLINE_ROOT_MONITOR=ON ..
make
make install
```
  ⚠️ Because of the bug discussed [here](https://github.com/eudaq/eudaq/pull/627), only the `master` branch of `eudaq` will work if `ROOT` was built using the C++17 standard.

  ⚠️ Omit the `-DEUDAQ_BUILD_ONLINE_ROOT_MONITOR=ON` if ROOT is not available on your system.
  
To add `eudaq` to the environment, the environmental variable `EUDAQ_DIR` needs to be set to the `eudaq` root directory
```bash
export EUDAQ_DIR=/path/to/eudaq
```

## Optional Dependencies
Most of these optional dependencies are for subcomponents of the [eudaq](eudaq) module and involve different methods of communication with subsystems of the detector.

### ROOT ([Docs](https://root.cern/install/build_from_source/))
As mentioned above, you will need to compile eudaq _after_ installing ROOT since eudaq uses ROOT for its online monitoring module.

We assume ROOT is available in this package. If ROOT is not availabe use the `-DBUILD_MONITORING=OFF` parameter when building ldmx-tb-online.

### pflib ([Docs](https://ldmx-software.github.io/pflib/))
This library is used for interacting with the Polarfire FPGA that communicates with the HGC ROC. (Read out chip for HCal and ECal).

Install command outline:
```bash
git clone https://github.com/LDMX-Software/pflib.git
cd pflib
git checkout v1.7 #or latest version
cmake -B build -S . -DCMAKE_INSTALL_PREFIX=install
cd build
make install
```
  ⚠️ You may need to tell `cmake` where `yaml-cpp` is with `-Dyaml-cpp_DIR=/full/path/to/yaml-cpp`.

If cmake is able to find pflib, this subcomponent of eudaq is built. 
If pflib is not installed to a system location, you will need to specify it with `-Dpflib_DIR=/full/path/to/pflib/install` 
or add the pflib install path to the `CMAKE_PREFIX_PATH` environment variable.

### dip + FiberTrackerDAQ ([Docs in README](https://github.com/pbutti/FiberTrackerDAQ))
This library is used for interacting with the hardware reading out the trigger scintillator.

Install command outline:
```
git clone https://github.com/pbutti/FiberTrackerDAQ
cd FiberTrackerDAQ
wget https://nexus.web.cern.ch/nexus/service/local/repositories/cern-nexus/content/cern/dip/dip/5.7.0/dip-5.7.0-distribution.zip
unzip dip-5.7.0.zip
cmake -DDIP_Dir=$(pwd)/dip-5.7.0/ -DCMAKE_INSTALL_PREFIX=. -B build -S .
cd build
make install
```

If the file `${FiberTrackerDAQ_DIR}/include/FiberTrackerClient.h` exists, then this subcomponent is built. The cmake variable `FiberTrackerDAQ_DIR` can be set on the command line with cmake or provided by the environment variable of the same name. We also require the cmake variable `dip_DIR` to be set to the directory with the unpacked contents of dip. If it is not set by the user, it is set to `${FiberTrackerDAQ_DIR}/dip-5.7.0/` which is where it is following the above command outline.

### WRTiming
This is used for interacting with the White Rabbit timing system.

If the file `${WRTiming_DIR}/include/WRClient.h` exists, then this subcomponent is built.
The cmake variable `WRTiming_DIR` can be set on the command line or provided by the environment variable of the same name.

## Building ldmx-tb-online

Once all dependencies are installed, building `ldmx-tb-online` is fairly straight forward
```bash
git clone git@github.com:LDMX-Software/ldmx-tb-online.git 
cd ldmx-tb-online/software
mkdir build; cd build
cmake .. # may need to provide additional arguments here depending on your available dependencies
make install
```
If the install prefix is not specified, `ldmx-tb-online` will be install in `../install` by default. 

## Setting up the Environment 

In order to run any `ldmx-tb-online` apps, the environment needs to be setup as
follows

```bash
export DAQ_INSTALL_PREFIX=/full/path/to/ldmx-tb-online/software/install
export LD_LIBRARY_PATH=$DAQ_INSTALL_PREFIX/lib:$LD_LIBRARY_PATH
export PATH=$DAQ_INSTALL_PREFIX/bin:$PATH
```

Once the commands above are executed, apps (e.g. emulator) can be run. 

# Running with EUDAQ

The following guide describes the procedure and elements needed to take
data using `EUDAQ`.  There are currently three possible ways to exercise
the full DAQ chain: 1) using emulated data 2) replaying raw data 
3) receiving raw data from a detector subsystem.  For initial testing, 
the use of 1 and 2 are recommended. 

At minimum, only the run control and `rogue` producer (`RogueTcpClientProducer`)
are needed to take data.  In this minimal case, the `RogueTcpClientProducer`
instantiates a `tcp` bridge that allows receiving of data from a server
on a specific port.  The raw data is then passed to a rogue file writer 
which dumps the data to disk.  The `RogueTcpClientProducer` also includes 
the logic to send commands to the server during the run control state 
transitions which enables initialization and configuration of a subdetector.

Each `EUDAQ` element needs to be run in a seperate terminal window.  This is 
denoted by the `Terminal #` specified prior to the command.  

To begin, the run control can be started as follows

Terminal 1 - Run Control
```bash
euRun -a tcp://4000
```

Terminal 2 - Producer
```bash
euCliProducer -n RogueTcpClientProducer -t hcal -r tcp://localhost:4000
```


## Emulation using eudaq

In addition to emulation using the stand alone app, eudaq can also be 
used.  Doing so will require starting the run control, producer, data collector, 
monitoring and and rogue server in seperate terminals.  This can be
done by executing the following commands (Note the terminal ID)

Terminal 1 - Run Control
```
euRun -n DarkRunControl -a tcp://4000
```

Terminal 2 - Producer
```
euCliProducer -n RogueTcpClientProducer -t hcal -r tcp://localhost:4000
```

Terminal 3 - Data Collector 
```
euCliCollector -n TestBeamDataCollector -t test_beam -r tcp://localhost:4000
```

Terminal 4 - Monitor
```
euCliMonitor -n SimpleMonitor -t mon -r tcp://localhost:4000
```

Terminal 5 - Rogue Server (HCal)
```
ldmx_rogue_server --hcal --emulate
```

Note, that both the `hcal` and `emulate` flags need to set on the server side.  Emulation 
of the trigger scintillator can be done by replacing `hcal` with `trig`.   It's also 
possible to emulate a producer for the trigger scintillator by instatiating 
an additional producer as follows

Terminal 6 - Trig Scint Producer
```
euCliProducer -n RogueTcpClientProducer -t trig -r tcp://localhost:4000
```

Terminal 7 - Rogue Server (Trigger Scint)

```
ldmx_rogue_server --trig --emulate --port 9000
```

Once the producers and run control has been started, you can start going through 
the state transitions.  An example of what is contained within an init file is as follows

```
[Producer.hcal]
TCP_ADDR = 127.0.0.1
TCP_PORT = 8000

[Producer.trig]
TCP_ADDR = 127.0.0.1
TCP_PORT = 9000
```

The `TCP_PORT` parameter should be set to whatever value was passed to the
rogue server. The default is 8000. 

The configure stage is what is used to connect the producers to the data 
collectors and monitoring. An example config file will look as follows
```
[Producer.hcal]
EUDAQ_DC=test_beam

[DataCollector.test_beam]
EUDAQ_MN=mon
```

The variable `EUDAQ_DC` is used to tell the producer the name of the data 
collector it should connect and send events to. Similarly, the variable
`EUDAQ_MN` is used to tell the data collector to which monitoring app
to connect to. 

### Legacy

There are two parts to the emulator: the server and client. The
server will make use of "Generators" to build frames and ship
them out at a specified rate via TCP/IP. The client will 
recieve the frames via TCP/IP and will pass it along to a receiver 
for further processing. Further documentation of how generators
and receivers interact via TCP/IP in rogue can be found 
[here](https://slaclab.github.io/rogue/interfaces/stream/usingTcp.html).
 

To run the Trigger Scintillator emulator, first execute the following
command on the server side

```
emulator --server --trig --rate 5
```

This will open a TCP/IP connection on all server interfaces using 
port 8000.  The `--rate` is used to specify the stream rate in Hz. 
The port can also be specified via the command line using the flag
`--port`.

Once the server side emulator has been started, the client side 
receiver can be initialize as follows

```
emulator --client --trig --addr <ip> --port <tcp/ip port> 
```

Where `<ip>` is the IP address of the server and `<tcp/ip port>` is
the port on which the tcp bridge was open on the server side.  If
both the server and client are running on the same machine, the 
address should be specified as `127.0.0.1`. 
 
Once the client is started, you should begin to see the RX and TX counters
start to increase. 

