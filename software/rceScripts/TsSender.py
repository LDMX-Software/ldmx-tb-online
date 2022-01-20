import rogue
import os,sys


class TsSender(rogue.interfaces.stream.Master):
    def __init__(self):
        super().__init__()
        

    def send_reply(self,reply):

        #100 bytes
        frame = self._reqFrame(100, True)
        
        encoded_string = reply.encode()
        ba = bytearray(encoded_string)

        frame.write(ba,0)


        print("Sending reply..." + reply)
        
        self._sendFrame(frame)

        print("Sent reply")
