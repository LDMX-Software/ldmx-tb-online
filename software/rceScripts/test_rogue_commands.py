import pyrogue
import rogue
import time

import TcpCommandSlave



def main():

    #Create a tcp bridge
    tcpServer = rogue.interfaces.stream.TcpServer('*',12345)
    
    #Create the tcp receiver
    tcp_rcv = TcpCommandSlave.TcpCommandSlave()

    tcpServer >> tcp_rcv
    
    while True:
        pass
        
if __name__=="__main__":
    main()
