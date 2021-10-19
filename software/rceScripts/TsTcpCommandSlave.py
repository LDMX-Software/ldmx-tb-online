import rogue
import os,sys
import TsSender
import time

class TsTcpCommandSlave(rogue.interfaces.stream.Slave):

    def __init__(self):
        super().__init__()
        self.state      = "idle"
        self.sender     = TsSender.TsSender()
        self.configPath =  "/home/ldmx/configurations"

    def msgSplit(self,msg,separator=":"):
        cmd_parts = msg.split(separator)
        
        if len(cmd_parts)<2:
            action = cmd_parts[0]
            content = [""]
        else:
            action  = cmd_parts[0]
            content = cmd_parts[1:]
            
        return (action,content)
        
    def _configure(self,msg):

        if (self.state != "idle" and self.state != "configured"):
            return

        config_file = (self.msgSplit(msg))[1]
        #bit ugly
        config_file=config_file[0]  

        if (len(config_file) < 1 or config_file==""):
            self.sender.send_reply("ERROR::Configuration file not present in the configuration file")
            return

        cFile = self.configPath +"/"+config_file
        
        if os.path.exists(cFile):
            #with open(cFile, 'r') as f:
                # Do Stuff with file
            print("Configuring the TS with " + cFile)

            time.sleep(5)
            self.sender.send_reply("INFO:: TS Configured with " + cFile)
            
        else:
            self.sender.send_reply("ERROR::Configuration file " + cFile + " doesn't exist.")
            
                    
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

        if ("configure" in msg or "config" in msg):
            self._configure(msg)

        elif (msg == "run"):
            self._run()

        elif (msg == "stop"):
            self._stop()

        else:
            print("Unknown request")
        
