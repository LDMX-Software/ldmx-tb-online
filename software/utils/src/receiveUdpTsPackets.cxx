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
#include <iomanip>



class TsUdpClient : public rogue::protocols::udp::Client {

private:
  
  const unsigned int RX_ADDR_OFFSET = 2;
  const unsigned int RX_DATA_OFFSET = 10;
  const unsigned int TX_DATA_OFFSET = 2;
  static const unsigned int MAXBUFLEN = 1492; // I think this is too much..
  
  unsigned char buff_[MAXBUFLEN];
  
public:
  static std::shared_ptr<TsUdpClient> create(std::string host, uint16_t port, bool jumbo) {
    std::shared_ptr<TsUdpClient> ptr =
      std::make_shared<TsUdpClient>(host,port,jumbo);
    return ptr;
  }
  
  TsUdpClient(std::string host, uint16_t port, bool jumbo);


  int getSocket(){return fd_;};
  
  int sendmsg(uint8_t* buffer, uint32_t size, bool dump=false);
  int32_t writereg(uint64_t addr, uint32_t value);
  int32_t readreg(uint64_t addr);
  void resetBuffer(){memset(buff_,0,sizeof(buff_));};
  void dumpBuffer(uint64_t size, uint64_t offset = 0);
  int32_t dataEnableToggle(bool enable);

  uint32_t setRemote();
  
};

  TsUdpClient::TsUdpClient (std::string host, uint16_t port, bool jumbo) : rogue::protocols::udp::Client(host, port, jumbo) {std::cout<<"Created TsUdpClient"<<std::endl;}


void TsUdpClient::dumpBuffer(uint64_t size, uint64_t offset){
  
  if (size > MAXBUFLEN) {
    std::cout<<"TsUdcpClient::dumpBuffer:: size>MAXBUFLEN.. Resize"<<std::endl;
    size=MAXBUFLEN;
  }
  
  for(int i = 0; i < size; ++i)  {
    std::cout << std::showbase // show the 0x prefix
	      << std::internal // fill between the prefix and the number
	      << std::setfill('0');
    
    std::cout << std::hex;
    std::cout<< (int)buff_[i]<<" ";
    //std::cout << (int)buff_[i];
    if ((i + offset) % 8 == 7)
      std::cout<<std::endl;
  }
  std::cout<<std::dec<<std::endl;
}

int TsUdpClient::sendmsg(uint8_t* buffer, uint32_t size, bool dump) {
  
  if (dump) {
    std::cout<<"::sending message::"<<std::endl;
    std::cout<<"==================="<<std::endl;
    dumpBuffer(size);
    std::cout<<"==================="<<std::endl;
  }
  
  int numbytes = sendto(fd_, buff_, size, 0, (struct sockaddr*)(&remAddr_), sizeof(remAddr_));
  if (numbytes == -1)
    std::cout<<"merda"<<std::endl;
  resetBuffer();
  std::cout<<"Returning "<< numbytes <<std::endl;
  return numbytes;
}

int32_t TsUdpClient::dataEnableToggle(bool enable){


  std::cout<<"data Enable Toggle"<<std::endl;
  buff_[0] = 1;
  buff_[1] = 1;
  uint64_t addr    = 0x0000000100000009;  // data enable
  memcpy((void*)&buff_[RX_ADDR_OFFSET], (void*)&addr, 8);
  memset((void*)&buff_[RX_DATA_OFFSET], 0, 8);
  memset((void*)&buff_[RX_DATA_OFFSET + 0], enable ? 1 : 0, 1);  // data toggle
  uint16_t size = RX_DATA_OFFSET + buff_[1] * 8;
  int numbytes = 0;
  
  numbytes = sendmsg(buff_,size,true);
  
  if ((int)numbytes<0) 
    std::cout<<"TsUdpClient::Data Enable Toggle::Error sending request to remote host"<<std::endl;
    return -1;
}

int32_t TsUdpClient::writereg(uint64_t addr, uint32_t value) {

    
  //if (sendmsg(buff_,size) == -1) {
  //  std::cout<<"TsUdpClient::Error sending request to remote host"<<std::endl;
  //  return -1;
  //}
}


int32_t TsUdpClient::readreg(uint64_t addr) {
  
  buff_[0] = 0; //read request
  buff_[1] = 1; //num of quadwords
  
  memcpy((void*)&buff_[RX_ADDR_OFFSET], (void*)&addr, 8);
  uint16_t size = RX_DATA_OFFSET;
  
  unsigned int numbytes;
    
  if (numbytes = sendmsg(buff_,size) == -1)  {
    std::cout<<"TsUdpClient::Error sending request to remote host"<<std::endl;
    return -1;
  }
    
  return 0;
}


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

  client->dataEnableToggle(false);
  client->dataEnableToggle(true);
  
  while (1){
  sleep(10);
  }

  client->dataEnableToggle(false);

  
  return 0;
  
}
