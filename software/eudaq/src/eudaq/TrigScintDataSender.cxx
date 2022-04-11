#include "eudaq/TrigScintDataSender.h"

#include <bitset>
#include <cstring>
#include <iostream>
//#include <inttypes.h>

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

  // Drop the first 2 byte containing the packet count. This is causing
  // words to be shifted and is going to break the parsing.
  // TODO(OM) Check again how many words need to be dropped.
  it += 2; 

  // Start buffering words until the end of event word 0xFFFFFFFFFFFFFFFF is 
  // reached.  Once the end of the end of the event is reached, create a 
  // eudaq event, ship it to the data collector and clear the buffer.   
  uint64_t word;

  while (it != frame->end()) { 
    rogue::interfaces::stream::fromFrame(it, 8, &word);

    // reorder the bytes when reading 8 bytes in a row
    // Apparently this is wrong. 
    //word = swapLong(word);
    
    //check the word being collected
    //std::cout<<"read word:"<<std::setw(16)<<std::setfill('0')<<std::hex<<word<<std::dec<<std::endl;
    //printf("%" PRIx64 "\n", word);

    if (word ==  0xFFFFFFFFFFFFFFFF) { 
      // The end of an event has been reached.  Clear everything, ship the event
      // out and  and prepare for a new event.

      // Create an eudaq event to ship to the data collector.
      auto event{eudaq::Event::MakeUnique("TrigScintRaw")};
    
      event->AddBlock(0x2, buffer);
      event->SetDeviceN(0x2); 

      // Send the event
      //std::cout<<"Data sender:: sending event"<<std::endl;

      //Remove the first corrupted event
      //at least 400 bytes. A typical event is 816 bytes.
      if (buffer.size() > 50) 
	producer_->SendEvent(std::move(event));
      buffer.clear();
    }
    
    else {
      buffer.push_back(word);
    }
  }
}
} // namespace eudaq
