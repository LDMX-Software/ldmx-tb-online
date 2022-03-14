#include "rogue/TsUdpClient.h"

#include <iomanip>
#include <iostream>
#include <string>


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

int TsUdpClient::sendmsg(uint32_t size, bool dump) {
  
  if (dump) {
    std::cout<<"::sending message::"<<std::endl;
    std::cout<<"==================="<<std::endl;
    dumpBuffer(size);
    std::cout<<"==================="<<std::endl;
  }
  
  int numbytes = sendto(fd_, buff_, size, 0, (struct sockaddr*)(&remAddr_), sizeof(remAddr_));
  resetBuffer();
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
  
  numbytes = sendmsg(size,true);
  
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
    
  if (numbytes = sendmsg(size) == -1)  {
    std::cout<<"TsUdpClient::Error sending request to remote host"<<std::endl;
    return -1;
  }
    
  return 0;
}


