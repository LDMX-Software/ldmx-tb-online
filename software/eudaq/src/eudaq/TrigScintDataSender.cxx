#include "eudaq/TrigScintDataSender.h"

#include <bitset>
#include <cstring>
#include <iostream>

#include "rogue/interfaces/stream/Frame.h"
#include "rogue/interfaces/stream/FrameIterator.h"

namespace eudaq {
void TrigScintDataSender::sendEvent(
    std::shared_ptr<rogue::interfaces::stream::Frame> frame) {

  // Get an iterator to the data in the frame
  auto it{frame->begin()};

  // Check for errors. If errors are found, skip processing of the frame.
  if (frame->getError()) {
    ++rx_error_count_;
    return;
  }

  // Drop the first 32 bit word that contains the event count. This is causing
  // words to be shifted and is going to break the parsing.
  // TODO(OM) Check again how many words need to be dropped.
  it += 4; 

  // Start buffering words until the end of event word 0xFFFFFFFFFFFFFFFF is 
  // reached.  Once the end of the end of the event is reached, create a 
  // eudaq event, ship it to the data collector and clear the buffer.   
  uint64_t word;

  while (it != frame->end()) { 
    rogue::interfaces::stream::fromFrame(it, 8, &word);
    
    if (word ==  0xFFFFFFFFFFFFFFFF) { 
      // The end of an event has been reached.  Clear everything, ship the event
      // out and  and prepare for a new event.

      // Create an eudaq event to ship to the data collector.
      auto event{eudaq::Event::MakeUnique("TrigScintRaw")};
    
      event->AddBlock(0x2, buffer);
      event->SetDeviceN(0x2); 

      // Send the event
      //event->Print(std::cout); 
      producer_->SendEvent(std::move(event));

      buffer.clear();
    }
  
    buffer.push_back(word); 
  }
}
} // namespace eudaq
