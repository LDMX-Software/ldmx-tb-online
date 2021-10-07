# Quickstart

## Dependencies

### Rogue 5.9.3
 * [Documentation](https://slaclab.github.io/rogue/index.html)
 * [Installng Rogue with Anaconda](https://slaclab.github.io/rogue/installing/anaconda.html)
 * [Installing Rogue on Archlinux](https://slaclab.github.io/rogue/installing/build.html#archlinux)

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
export DAQ_INSTALL_PREFIX=/full/path/to/ldmx-daq/install
export LD_LIBRARY_PATH=$DAQ_INSTALL_PREFIX/lib:$LD_LIBRARY_PATH
export PATH=$DAQ_INSTALL_PREFIX/bin:$PATH
```

Once the commands above are executed, apps (e.g. emulator) can be run. 

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
