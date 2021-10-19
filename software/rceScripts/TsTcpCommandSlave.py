import rogue
import os,sys
import TsSender

class TsTcpCommandSlave(rogue.interfaces.stream.Slave):

    def __init__(self):
        super().__init__()
        self.state = "idle"
        self.sender = TsSender.TsSender()


    def _configure(self,config_file):

        if (self.state != "idle" and self.state != "configured"):
            return

        #load configuration
        self.state = "configured"

    def _run(self):
        self.state = "run"

    def _stop(self):
        self.state = "stop"
        
    def _acceptFrame(self,frame):

        with frame.lock():
            size = frame.getPayload()
            print("Message payload " + str(size))
            fullData = bytearray(size)
            frame.read(fullData,0)

        msg = fullData.decode('UTF-8')
        print("Message: "+ msg)
        
        if (msg == "configure"):
            self._configure()

        elif (msg == "run"):
            self._run()

        elif (msg == "stop"):
            self._stop()

        else:
            print("Unknown request")
        
