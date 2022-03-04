import pyrogue
import rogue
import time

#Master to send the data from the TS to the runControl
class TsDataSender(rogue.interfaces.stream.Master):
    def __init__(self):
        super().__init__()
        
    def genFrame(self,size):
        
        payload = 124 #31*32/8
        
        #100 bytes
        frame = self._reqFrame(payload,True)

        #ba is the bytearray to be sent back
        
        frame.write(ba,0)
        
        self._sendFrame(frame)
        
