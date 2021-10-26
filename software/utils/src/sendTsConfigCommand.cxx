#include <string>
#include <iostream>
#include "eudaq/TcpCommandGenerator.h"
#include "rogue/TrigScintReceiver.h"
#include "rogue/interfaces/stream/TcpClient.h"

using namespace eudaq;

int main(int argc, char* argv[]) {

  std::cout<<"Test TcpCommandGenerator "<<std::endl;
  
  //Open the TCP bridge
  rogue::interfaces::stream::TcpClientPtr tcp_ = rogue::interfaces::stream::TcpClient::create("192.168.1.50",10000);

  //Create the TCP command sender
  std::shared_ptr<TcpCommandGenerator> tcp_command_{TcpCommandGenerator::create()};
  tcp_command_->addSlave(tcp_);
  
  //Create the TCP command read-back receiver
  std::shared_ptr<TrigScintReceiver> tcp_receiver_{TrigScintReceiver::create()};
  tcp_->addSlave(tcp_receiver_);

  //tcp_command_->send_cmd("configure:dummy");
  tcp_command_->genFrame("configure:dummy2");
  
    
  while (1) {
   
  };
  
  //tcp_command_->send_var("motorPosX",1500);
  //tcp_command_->send_var("motorPosX",-1500);

}

