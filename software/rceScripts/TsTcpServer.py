import pyrogue
import rogue
import time

import TsTcpCommandSlave


class TsTcpServer(object):
    def __init__(self):
        print("Starting TsTcpServer")

        #Create a tcp bridge
        self.tcp = rogue.interfaces.stream.TcpServer('*',10000)
        
        #create local receiver
        self.tcp_rcv = TsTcpCommandSlave.TsTcpCommandSlave()

        #Connect the tcp receiver and the tcp reply sender
        self.tcp_rcv.sender >> self.tcp >> self.tcp_rcv

        print("Server connected...")


def main():

    rogueTsServer = TsTcpServer()

    try:
        while True:
            if (rogueTsServer.tcp_rcv.state=="configured"):
                pass    

    except KeyboardInterrupt:
        print("Stopping TsTcpServer")
            

        
if __name__=="__main__":
    main()

    
    
