import rogue
import os, sys
sys.path.append('/home/whitbeck/testBeam/')
import SystemMonitor.VXM.vxmtools as vxm
import Hamamatsu.serialInterface as uart

class TcpCommandSlave(rogue.interfaces.stream.Slave):
    # Init method must call the parent class init
    def __init__(self):
        super().__init__()
        
        self.state = "idle"
        vxm.Init()
        TS_power = uart.serialInterface('/dev/ttyUSB1',False)

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

    def _setVariable(self,msg):
        cmd_parts = msg.split(":");
        if (len(cmd_parts))<3:
            print("SetVariable Command is malformed.. do nothing")
            return
        var_name  = cmd_parts[1]
        var_value = float(cmd_parts[2])
        
        print(var_name + " " + str(var_value))

        if var_name == 'BV' :
            print('setting BV...')
            TS_power.referencevoltageSetting(var_value)
            print('BV: ',TS_power.voltageOut())
            
        elif var_name == 'motorPosX' :
            print(var_value)
            vxm.move(var_value,var_value)
            pos = vxm.GetPos()
            print(pos)
            
        else :
            print('i do not know what to do...')
        
    def _getMotorPos(self):
        print("success")
        pos = vxm.GetPos()
        print(pos)
        
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

        elif (msg == "run"):
            self._run()

        elif (msg == "stop"):
            self._stop()

        elif (msg == "getMotorPos"):
            self._getMotorPos()
            
        elif ("setVariable" in msg):
            self._setVariable(msg)

        else:
            print("Unknown request")
        
