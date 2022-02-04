#ifndef ROGUE_CAPTANTSRECEIVER
#define ROGUE_CAPTANTSRECEIVER

#include "rogue/protocols/udp/Client.h"
#include <rogue/protocols/udp/Core.h>


class TsUdpClient : public rogue::protocols::udp::Client {
  
 private:
  
  const unsigned int RX_ADDR_OFFSET = 2;
  const unsigned int RX_DATA_OFFSET = 10;
  const unsigned int TX_DATA_OFFSET = 2;
  static const unsigned int MAXBUFLEN = 1492;
  
 public:

  char buff_[MAXBUFLEN];
    
  static std::shared_ptr<TsUdpClient> create(std::string host, uint16_t port, bool jumbo) {
    std::shared_ptr<TsUdpClient> ptr =
      std::make_shared<TsUdpClient>(host,port,jumbo);
    return ptr;
  }
  
  TsUdpClient(std::string host, uint16_t port, bool jumbo);


  int getSocket(){return fd_;};
  
  
  int sendmsg(uint32_t size, bool dump=false);
  int32_t writereg(uint64_t addr, uint32_t value);
  int32_t readreg(uint64_t addr);
  void resetBuffer(){memset(buff_,0,sizeof(buff_));};
  void dumpBuffer(uint64_t size, uint64_t offset = 0);
  int32_t dataEnableToggle(bool enable);
  
  
};


#endif
