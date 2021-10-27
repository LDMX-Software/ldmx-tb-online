#include <string>
#include <iostream>
#include <unistd.h>
#include "rogue/protocols/udp/Client.h"
#include <rogue/protocols/udp/Core.h>



#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>



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
  std::string addr=argv[1];
  uint16_t port=2007;

  //No jumbo frames
  //std::shared_ptr<TsUdpClient> client = TsUdpClient::create(addr,port,false);

  rogue::protocols::udp::ClientPtr client = rogue::protocols::udp::Client::create(addr,port,false);
  //std::cout<<"client created"<<std::endl;
  //std::cout<<"listening on "<< addr<<std::endl;
  //std::cout<<"port "<<port<<std::endl;

  while (1){
  sleep(1);
  }
  /*

  const char* hostname="127.0.0.1"; 
  const char* portname = "9999";
  struct addrinfo hints;
  memset(&hints,0,sizeof(hints));
  hints.ai_family=AF_INET;
  hints.ai_socktype=SOCK_DGRAM;
  hints.ai_protocol=0;
  hints.ai_flags=AI_PASSIVE|AI_ADDRCONFIG;
  struct addrinfo* res=0;
  int err=getaddrinfo(hostname,portname,&hints,&res);
  if (err!=0) {
    printf("failed to resolve local socket address (err=%d)",err);
  }
  
  int fd=socket(res->ai_family,res->ai_socktype,res->ai_protocol);
  if (fd==-1) {
    printf("%s",strerror(errno));
  }

  if (bind(fd,res->ai_addr,res->ai_addrlen)==-1) {
    printf("%s",strerror(errno));
  }
  
  char buffer[549];
  struct sockaddr_storage src_addr;
  socklen_t src_addr_len=sizeof(src_addr);

  while (1) {
    ssize_t count=recvfrom(fd,buffer,sizeof(buffer),0,(struct sockaddr*)&src_addr,&src_addr_len);
    if (count==-1) {
      printf("%s",strerror(errno));
    } else if (count==sizeof(buffer)) {
      printf("datagram too large for buffer: truncated");
    } else {
      std::cout<<"Received "<<count<<" bytes"<<std::endl;
    }
  }

  */
  
  return 0;
  
}
