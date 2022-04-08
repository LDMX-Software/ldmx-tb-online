#include "TrigScintTestBeamMonitor.h"

#include <iostream>

namespace {
auto dummy0 = eudaq::Factory<eudaq::Monitor>::Register<
    eudaq::TrigScintTestBeamMonitor, const std::string &, const std::string &>(
    eudaq::TrigScintTestBeamMonitor::factory_id_);
}

namespace eudaq {

void TrigScintTestBeamMonitor::AtConfiguration() {
  /**
   * Book plots here.
   */
}

void TrigScintTestBeamMonitor::AtEventReception(EventSP event) {
  /**
   * Drop code to extract samples here.
   */

  //0x2 is correct?
  //The getBlock will return a buffer of bytes
  std::vector<uint8_t> buffer = event->GetBlock(0x2);
  // buffers for individual words for each data fiber
  // 16 bit buffers are convenient for removing pad
  // words from the data stream
  uint16_t fiber1a,fiber2a,fiber1b,fiber2b;
  //std::cout<<"Received event, buffer size::" << buffer.size()<<std::endl;
  
  for (unsigned int i=0; i < buffer.size();i+=8 ) {
    fiber1a = buffer[i]   << 8 | buffer[i+1];
    fiber2a = buffer[i+2] << 8 | buffer[i+3];
    fiber1b = buffer[i+4] << 8 | buffer[i+5];
    fiber2b = buffer[i+6] << 8 | buffer[i+7];
    
    //std::cout<<std::setw(4)<<std::setfill('0')<<std::hex<<fiber1a<<" "<<fiber2a<<" "<<fiber1b<<" "<<fiber2b<<std::dec<<std::endl;
    
    // remove all comma chraacters before filling buffer
    
    // Slow
    if( fiber1a != comma_char)
      event_buffer1.push_back(fiber1a);
    
    if( fiber1b != comma_char )
      event_buffer1.push_back(fiber1b);
    
    if( fiber2a != comma_char )
      event_buffer2.push_back(fiber2a);
    
    if( fiber2b != comma_char )
      event_buffer2.push_back(fiber2b);
   
    //Slow
    event_buffer1.clear();
    event_buffer2.clear();
    
    
  }
  //Check that the length of the event buffers is correct
  
  if (event_buffer1.size() != event_buffer2.size())
    std::cout<<"Error in decoding. Event buffers have different length"<<std::endl;
  

  
  
}
} // namespace eudaq
