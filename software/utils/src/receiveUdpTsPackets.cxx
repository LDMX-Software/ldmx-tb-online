#include <string>
#include <iostream>
#include <unistd.h>
#include "rogue/protocols/udp/Client.h"
#include <rogue/protocols/udp/Core.h>

/*
class TsUdpClient : public rogue::protocols::udp::Client {

 public:
  static std::shared_ptr<TsUdpClient> create(std::string host, uint16_t port, bool jumbo) {
    std::shared_ptr<TsUdpClient> ptr =
        std::make_shared<TsUdpClient>(host,port,jumbo);
    return ptr;
  }

  TsUdpClient(std::string host, uint16_t port, bool jumbo);
  int32_t getSocket(){return fd_;};
};

TsUdpClient::TsUdpClient (std::string host, uint16_t port, bool jumbo) : rogue::protocols::udp::Client(host, port, jumbo) {std::cout<<"Created TsUdpClient"<<std::endl;};
*/

/** Test the UDP data receiver from the CAPTAN-X Board **/


int main(int argc, char* argv[]) {

  std::cout<<"Test UDP Captan-X TS data reader" <<std::endl;
  std::string addr="127.0.0.1";
  uint16_t port=9999;

  //No jumbo frames
  //std::shared_ptr<TsUdpClient> client = TsUdpClient::create(addr,port,false);

  rogue::protocols::udp::ClientPtr client = rogue::protocols::udp::Client::create(addr,port,false);
  std::cout<<"client created"<<std::endl;
  std::cout<<"listening on "<< addr<<std::endl;
  std::cout<<"port "<<port<<std::endl;
  
  //Try to get data over the socket
  //ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
  //                 struct sockaddr *src_addr, socklen_t *addrlen);
  
  while (1) {
    sleep(1);
  }
      
  return 0;
  
}
