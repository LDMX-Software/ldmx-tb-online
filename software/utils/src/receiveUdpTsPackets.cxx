#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <memory>
#include "rogue/TsUdpClient.h"

/** Test the UDP data receiver from the CAPTAN-X Board **/


int main(int argc, char* argv[]) {

  std::cout<<"Test UDP Captan-X TS data reader" <<std::endl;
  std::string addr=argv[1];
  uint16_t port=atoi(argv[2]);

  //No jumbo frames
  std::shared_ptr<TsUdpClient> client = TsUdpClient::create(addr,port,false);

  //rogue::protocols::udp::ClientPtr client = rogue::protocols::udp::Client::create(addr,port,false);
  std::cout<<"client created"<<std::endl;
  //std::cout<<"listening on "<< addr<<std::endl;
  std::cout<<"port "<<port<<std::endl;

  //client->readreg(6);

  client->buff_[0]=2;
  client->sendmsg(1,true);
  
  client->dataEnableToggle(false);
  
  client->dataEnableToggle(true);
  
  while (1){
  sleep(10);
  }

  client->dataEnableToggle(false);

  
  return 0;
  
}
