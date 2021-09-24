import pyrogue
import rogue
import time


import socket
import ipaddress
import pyrogue.utilities.prbs
import rogue.interfaces.stream

pyrogue.addLibraryPath('/mnt/hps/heavy-photon-daq/firmware/common/HpsDaq/python')
pyrogue.addLibraryPath('/mnt/hps/heavy-photon-daq/firmware/submodules/surf/python')
pyrogue.addLibraryPath('/mnt/hps/heavy-photon-daq/firmware/submodules/rce-gen3-fw-lib/python')

import RceG3

class RceRoot(pyrogue.Root):
    def __init__(self, memBase, **kwargs):
        super().__init__(**kwargs)
        print('Building RceRoot')

        self.add(RceG3.RceVersion(memBase=memBase)) # Statically mapped at 0x80000000
        self.add(RceG3.RceEthernet(memBase=memBase, offset=0xB0000000))


class TcpCommandSlave(rogue.interfaces.stream.Slave):
    # Init method must call the parent class init
    def __init__(self):
        super().__init__()
        
        self.state = "idle"


    def _configure(self):
        
        #Protect against sending configuration if already running
        if (self.state != "idle" and self.state != "configured"):
            return
            
        if (self.state == "configured"):
            print("Hardware already configured.. Will re-send configuration")
        else:
            print("Sending configuration to hardware...")
        
        self.state = "configured"


    def _run(self):
        self.state = "run"

    def _stop(self):
        self.state = "stop" 

    def _acceptFrame(self,frame):

        # First it is good practice to hold a lock on the frame data.
        with frame.lock():
            # Next we can get the size of the frame payload
            size = frame.getPayload()
            print("Message payload " + str(size))
            # To access the data we need to create a byte array to hold the data
            fullData = bytearray(size)

            # Next we read the frame data into the byte array, from offset 0
            frame.read(fullData,0)

        msg = fullData.decode('UTF-8')
        print("Message: "+ msg)

        if (msg == "configure"):
            self._configure()

        if (msg == "run"):
            self._run()

        if (msg == "stop"):
            self._stop()
        
class LdmxRceTcpServer(object):

    def __init__(self):

        rogue.Logging.setFilter('pyrogue.stream.TcpCore', rogue.Logging.Warning)    #debug
        rogue.Logging.setFilter('pyrogue.memory.TcpServer', rogue.Logging.Warning)


        print('Starting RceTcpServer')

        #Create the local receiver
        self.tcp_rcv = TcpCommandSlave()
                
        #rogue.Logging.setFilter('pyrogue.stream.TcpCore', rogue.Logging.Debug)

        # Define the memory map
        self.memMap = rogue.hardware.axi.AxiMemMap('/dev/rce_memmap')

        # Memory server on port 12000
        ##MAKE IT CONFIGURABLE##
        RCE_MEM_MAP_PORT = 12000
        self.memServer = rogue.interfaces.memory.TcpServer('*', RCE_MEM_MAP_PORT)
        pyrogue.busConnect(self.memServer, self.memMap)
        print(f'Opened Memory TcpServer on port {RCE_MEM_MAP_PORT}')

        # Spin up a Root to querry the BSI and set the IP address for firmware
        with RceRoot(memBase=self.memMap) as root:
            # Set the IP address
            buildStamp = root.RceVersion.BuildStamp.get(read=True)
            time.sleep(1)
            imageName = root.RceVersion.ImageName.get(read=True)
            print(f'RCE Firmware: {buildStamp}')
            print(f'image name: {imageName}')

            if imageName == 'DataDpm':
                ipAddress = ipaddress.IPv4Address(socket.gethostbyname(socket.gethostname())).exploded
                root.RceEthernet.IpAddress.set(ipAddress, write=True)
                print(f'RceEthernet.IpAddress: {ipAddress}')

                #Connect a prbs to tcp
                self.dpmPrbsDataTcpServer = rogue.interfaces.stream.TcpServer('*',8000)

                #Create a prbs stream
                self.prbs_src = rogue.utilities.Prbs()

                #Connect the trasmitter to the TcpServer
                self.prbs_src >> self.dpmPrbsDataTcpServer
                
                #Connect the tcp receiver and the tcp bridge

                self.dpmPrbsDataTcpServer >> self.tcp_rcv
                
                #Connect the dpmDataDmaChannel to tcp
                
                #self.dpmDataDmaChannel = rogue.hardware.axi.AxiStreamDma('/dev/axi_stream_dma_0', 0, True)
                #self.dpmDataTcpServer = rogue.interfaces.stream.TcpServer('*', 12345)
                #self.dpmDataDebug = rogue.interfaces.stream.Slave()
                #pyrogue.streamConnectBiDir(self.dpmDataDmaChannel, self.dpmDataTcpServer)
                #pyrogue.streamTap(self.dpmDataDmaChannel, self.dpmDataDebug)
    



def dumpPrbsRunInfo(prbs_master):

    print(" --- Run Summary ---")
    print("Tx Frames Sent: " + str(prbs_master.getTxCount()))
    print("Tx Rate       : " + str(prbs_master.getTxRate()))
    print("Tx Bytes Sent : " + str(prbs_master.getTxBytes()))
    print("Tx Bandwidth  : " + str(prbs_master.getTxBw()))
    
            
if __name__ == "__main__":

    tcpServer = LdmxRceTcpServer()
    print("LdmxRceTcpServer is up")
    cfg_print = False
    try:
        #Keep server alive
        while True:
            
            if (tcpServer.tcp_rcv.state == "configured"):
                if (not cfg_print):
                    print("HW is now configured")
                    cfg_print = True
            
            if (tcpServer.tcp_rcv.state == "run"):
                tcpServer.prbs_src.genFrame(1000)
            
            if (tcpServer.tcp_rcv.state == "stop"):
                print("Stopping the run...")
                print("Setting receiver to idle state...")
                
                dumpPrbsRunInfo(tcpServer.prbs_src);
                tcpServer.prbs_src.resetCount()
                tcpServer.tcp_rcv.state = "idle"

                
            #1ms   (2kHz)
            time.sleep(1./2000.)



            
    except KeyboardInterrupt:
        print("Stopping LdmxRceTcpServer")

    
