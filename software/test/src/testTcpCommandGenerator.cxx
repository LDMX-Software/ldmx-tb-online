#include <string>
#include <iostream>
#include "eudaq/TcpCommandGenerator.h"
#include "rogue/interfaces/stream/TcpClient.h"

using namespace eudaq;

int main(int argc, char* argv[]) {

  std::cout<<"Test TcpCommandGenerator "<<std::endl;
  
  //Open the TCP bridge
  rogue::interfaces::stream::TcpClientPtr tcp_ = rogue::interfaces::stream::TcpClient::create("192.168.1.22",12345);
  std::shared_ptr<TcpCommandGenerator> tcp_command_{TcpCommandGenerator::create()};

  tcp_command_->addSlave(tcp_);

  tcp_command_->send_var("BiasVoltage",5.5);
  tcp_command_->send_cmd("getMotorPos");
  tcp_command_->send_var("motorPosX",1500);
  tcp_command_->send_var("motorPosX",-1500);
  
}

