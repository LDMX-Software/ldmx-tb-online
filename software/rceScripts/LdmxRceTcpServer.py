import pyrogue
import rogue
import time
import pyrogue.utilities.prbs
import rogue.interfaces.stream

class LmdxRceRoot(pyrogue.Root):
    def __init__(self, memBase, **kwargs):
        super().__init__(**kwargs)
        print('Building RceRoot')

        self.add(RceG3.RceVersion(memBase=memBase)) # Statically mapped at 0x80000000
        self.add(RceG3.RceEthernet(memBase=memBase, offset=0xB0000000))


class LmdxRceTcpServer(object):

    def __init__(self):

        rogue.Logging.setFilter('pyrogue.stream.TcpCore', rogue.Logging.Debug)
        rogue.Logging.setFilter('pyrogue.memory.TcpServer', rogue.Logging.Debug)


        print('Starting RceTcpServer')

        #rogue.Logging.setFilter('pyrogue.stream.TcpCore', rogue.Logging.Debug)

        # Define the memory map
        self.memMap = rogue.hardware.axi.AxiMemMap('/dev/rce_memmap')

        # Memory server on port 9000
        self.memServer = rogue.interfaces.memory.TcpServer('*', hps.constants.RCE_MEM_MAP_PORT)
        pyrogue.busConnect(self.memServer, self.memMap)
        print(f'Opened Memory TcpServer on port {hps.constants.RCE_MEM_MAP_PORT}')

        # Spin up a Root to querry the BSI and set the IP address for firmware
        with LdmxRceRoot(memBase=self.memMap) as root:
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
                
                
                #Connect the dpmDataDmaChannel to tcp
                
                #self.dpmDataDmaChannel = rogue.hardware.axi.AxiStreamDma('/dev/axi_stream_dma_0', 0, True)
                #self.dpmDataTcpServer = rogue.interfaces.stream.TcpServer('*', 12345)
                #self.dpmDataDebug = rogue.interfaces.stream.Slave()
                #pyrogue.streamConnectBiDir(self.dpmDataDmaChannel, self.dpmDataTcpServer)
                #pyrogue.streamTap(self.dpmDataDmaChannel, self.dpmDataDebug)
            

            

