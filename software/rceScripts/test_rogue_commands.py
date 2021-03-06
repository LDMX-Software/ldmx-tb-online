import pyrogue
import rogue
import time
import sys,os
import TcpCommandSlave
import TsSender


def main():

    #Create a tcp bridge
    tcpServer = rogue.interfaces.stream.TcpServer('*',12345)
    
    #Create the tcp receiver
    tcp_rcv = TcpCommandSlave.TcpCommandSlave()
    
    #Connect the tcp receiver and the tcp reply sender
    tcp_rcv.sender >> tcpServer >> tcp_rcv

    print("Starting Server...",flush=True)
    
    while True:
        pass
        
if __name__=="__main__":
    main()
