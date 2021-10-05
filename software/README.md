# Building ldmx-daq

## Dependencies

* Rogue
  * Building Rogue with Anaconda
  * Building Rogue on Archlinux

## Build

Start by cloning the ldmx-daq repository and making a build directory
within the software directory

git clone git@github.com:slaclab/ldmx-daq.git 
cd ldmx-daq/software; mkdir build; cd build;

If Rogue was installed with Anaconda, building ldmx-daq will require
first enabling the rogue conda environment as follows

conda activate rogue_dev

Configuring the build via CMake can be done withing the build directory
as follows

cmake -DCMAKE_INSTALL_PREFIX=../install ..

After the cmake step exits without errors, you can build and install 
ldmx-daq with the following command

make install

This will build and install ldmx-daq into the install directory specified 
above.

## Setting up the Environment 

In order to run any ldmx-daq apps, the environment needs to be setup as
follows

export DAQ_INSTALL_PREFIX=/full/path/to/ldmx-daq/install
export LD_LIBRARY_PATH=$DAQ_INSTALL_PREFIX/lib:$LD_LIBRARY_PATH
export PATH=$DAQ_INSTALL_PREFIX/bin:$PATH

Once executed, apps such as the emulator can be run. 

# Running the emulator

There are two parts to the emulator: the server and a client. The
server will make use of "Generators" to build frames and ship
them out at a specified rate via TCP/IP. The client will 
recieve the frames via TCP/IP and will pass it along to a receiver 
for further processing. 

To run the Trigger Scintillator emulator, first execute the following
command on the server side

emulator --server --trig

This will open a TCP/IP connection on all server interfaces using 
port 8000.  You can also specify the port via the command line using
--port. The rate in Hz can be specified using the --rate flag. 

Once the server side emulator has been started, the client side 
receiver can be initialize as follows

emulator --client --trig --addr <server ip> --port <tcp/ip port> 

Once the client is started, you should begin to see the RX and TX counters
start to increase. 
