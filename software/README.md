# Quickstart

## Dependencies

### Rogue 5.9.3
 * [Documentation](https://slaclab.github.io/rogue/index.html)
 * [Installng Rogue with Anaconda](https://slaclab.github.io/rogue/installing/anaconda.html)
 * [Installing Rogue on Archlinux](https://slaclab.github.io/rogue/installing/build.html#archlinux)

### eudaq 
 * [Documentation](https://eudaq.github.io/) 

## Building ldmx-daq

First, clone the `ldmx-daq` repository and make a build directory
within the software directory

```
git clone git@github.com:slaclab/ldmx-daq.git 
cd ldmx-daq/software; mkdir build; cd build;
```

If `Rogue` was installed with Anaconda, the rogue environment needs
to be first enabled as follows

```
conda activate rogue_dev
```

Note, `rogue_dev` is the name of the conda environment specified during the
installation step.  If rogue was installed baremetal e.g. on Archlinux, 
the conda step is not required.

Configuring the build via CMake can be done withing the build directory
as follows

```
cmake -DCMAKE_INSTALL_PREFIX=../install ..
```
In this case, the install prefix was specified as `../install`.  If the 
install prefix is not specified, ldmx-daq will be install in `../install`
by default. After the cmake step exits without errors, you can build and 
install `ldmx-daq` with the following command

```
make install
```

This will build and install ldmx-daq into the install directory specified 
above.

## Setting up the Environment 

In order to run any `ldmx-daq` apps, the environment needs to be setup as
follows

```
export DAQ_INSTALL_PREFIX=/full/path/to/ldmx-daq/software/install
export LD_LIBRARY_PATH=$DAQ_INSTALL_PREFIX/lib:$LD_LIBRARY_PATH
export PATH=$DAQ_INSTALL_PREFIX/bin:$PATH
```

Once the commands above are executed, apps (e.g. emulator) can be run. 

Eudaq loads all run control, producer and data collector libraries at 
run time.  For this reason, it needs to be made aware of the LDMX 
specific eudaq libraries.  This can be done by creating a soft link
of the LDMX eudaq library within the eudaq `lib` directory as follows

```
ln -s /full/path/to/ldmx-daq/software/install/lib/libeudaq_module_dark.so /full/path/to/eudaq/lib/libeudaq_module_dark.so
```

# Running the emulator

There are two parts to the emulator: the server and client. The
server will make use of "Generators" to build frames and ship
them out at a specified rate via TCP/IP. The client will 
recieve the frames via TCP/IP and will pass it along to a receiver 
for further processing. Further documentation of how generators
and receivers interact via TCP/IP in rogue can be found 
[here](https://slaclab.github.io/rogue/interfaces/stream/usingTcp.html).

## Example - Trigger Scintillator Emulator 

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

## Emulation using eudaq

In addition to emulation using the stand alone app, eudaq can also be 
used.  Doing so will require starting the run control, producer and rogue 
server in seperate terminals as follows

Terminal 1
```
euRun -n DarkRunControl -a tcp://4000
```

Terminal 2
```
euCliProducer -n RogueTcpClientProducer -t hcal -r tcp://localhost:4000
```

Terminal 3
```
ldmx_rogue_server --hcal --emulate
```

Note, that both the `hcal` and `emulate` flags need to set on the server side.  Emulation 
of the trigger scintillator can be done by replacing `hcal` with `trig`.   It's also 
possible to emulate a producer for the trigger scintillator by instatiating 
an additional producer as follows

Terminal 4
```
euCliProducer -n RogueTcpClientProducer -t trig -r tcp://localhost:4000
```

```
ldmx_rogue_server --trig --emulate --port 9000
```

Once the producers and run control has been started, you can start going through 
the state transitions. Currently, only the init file will have parameters inside.  
An example of what is contained within an init file is as follows

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

