import pyrogue
import rogue
import pyrogue.utilities.prbs
import pyrogue.protocols
import time

class PrbsSourceRoot(pyrogue.Root):

    def __init__(self):

        pyrogue.Root.__init__(self,
                              name='PrbsSourceRoot',
                              description="PRBS Source Root",
                              pollEn=True,
                              serverPort=8900)

        prbsTx = pyrogue.utilities.prbs.PrbsTx(expand=True)
        udp    = pyrogue.protocols.UdpRssiPack(jumbo=True, packVer=2, server=True, port=8800)
        
        prbsTx >> udp.application(0)

        self.add(prbsTx)
        self.add(udp)


if __name__ == "__main__":

    
    root = PrbsSourceRoot()

    while (1):
        
        print("Gen Frame: 1")
        root.PrbsTx._genFrame(1)
        time.sleep(5)

        
    
