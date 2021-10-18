#ifndef EUDAQ_TSRECEIVER_H
#define EUDAQ_TSRECEIVER_H

#include <string>

#include "rogue/interfaces/stream/Slave.h"

namespace eudaq {
  
  class TsReceiver : public rogue::interfaces::stream::Slave {
  public:
    static std::shared_ptr<TsReceiver> create() {
      static std::shared_ptr<TsReceiver> ret =
	std::make_shared<TsReceiver>();
      return (ret); 
    }
    
    TsReceiver() = default;
    ~TsReceiver() = default;

    void acceptFrame(std::shared_ptr<rogue::interfaces::stream::Frame> frame);
    std::string convertToString(char* a, int size);
    
  };
}

#endif

    

