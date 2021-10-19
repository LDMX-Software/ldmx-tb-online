import rogue
import os, sys
sys.path.append('/home/whitbeck/testBeam/')
from SystemMonitor.VXM.vxmtools import *
import Hamamatsu.serialInterface as uart

import TsSender

class TcpCommandSlave(rogue.interfaces.stream.Slave):
    # Init method must call the parent class init
    def __init__(self):
        super().__init__()
        
        self.state = "idle"
        vxm_ = vxm()
        self.TS_power = uart.serialInterface('/dev/ttyUSB1',False)
        self.sender = TsSender.TsSender()

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

        if var_name == 'BiasVoltage' :
            print('setting BV...')
            self.TS_power.referencevoltageSetting(var_value)
            self.sender.send_reply('BV:{0} current:{1} status: {2} '.format(self.TS_power.voltageOut(),self.TS_power.currentOut(),self.TS_power.getStatus()))

        elif var_name == 'motorPosX' :
            print(var_value)
            vxm_.move(var_value,0)
            pos = vxm_.GetPos()
            pos_string= [str(i) for i in pos ]
            pos_string=",".join(pos_string)
            print(pos_string)
            self.sender.send_reply(pos_string)

        elif var_name == 'motorPosY' :
            print(var_value)
            vxm_.move(0,var_value)
            pos = vxm_.GetPos()
            pos_string= [str(i) for i in pos ]
            pos_string=",".join(pos_string)
            print(pos_string)
            self.sender.send_reply(pos_string)

        elif var_name == 'motorPosXY' :
            if (len(cmd_parts))<4:
                print("SetVariable Command is malformed.. do nothing")
                return
            X = float(cmd_parts[2])
            Y = float(cmd_parts[3])
            vxm_.move(0,var_value)
            pos = vxm_.GetPos()
            pos_string= [str(i) for i in pos ]
            pos_string=",".join(pos_string)
            print(pos_string)
            self.sender.send_reply(pos_string)

        else :
            print('i do not know what to do...')
        
    def _getMotorPos(self):
        print("success")
        pos = vxm_.GetPos()
        print(pos)
        pos_string= [str(i) for i in pos ]
        pos_string=",".join(pos_string)
        print(pos_string)
        self.sender.send_reply(pos_string)
        
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
        
