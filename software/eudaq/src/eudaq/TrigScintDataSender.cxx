#include "eudaq/TrigScintDataSender.h"

//---< C++ StdLib >---//
#include <bitset>
#include <cstring>
#include <iostream>

//---< rogue >---//
#include "rogue/interfaces/stream/Frame.h"
#include "rogue/interfaces/stream/FrameIterator.h"

namespace eudaq {
void TrigScintDataSender::sendEvent(
    std::shared_ptr<rogue::interfaces::stream::Frame> frame) {

  // Get an iterator to the data in the frame
  auto it{frame->begin()};

  // Extract the trigger number from the header.
  uint8_t word;
  
  rogue::interfaces::stream::fromFrame(it, 1, &word);
  auto trigger_id{255*word}; 
  rogue::interfaces::stream::fromFrame(it, 1, &word);
  trigger_id += word; 

  // Create an eudaq event to ship to the data collector. At this point,
  // the trigger number is set using the RREQ parameter in the HGCROC
  // data structure.
  auto event{eudaq::Event::MakeUnique("TrigScintTestBeamRaw")};
  event->SetTriggerN(trigger_id); 

  // Copy the data block from the rogue frame
  it = frame->begin();
  event->AddBlock(0x2, it.ptr(), frame->getPayload()); 

  //event->Print(std::cout); 
  // Send the event to the data collectors
  producer_->SendEvent(std::move(event)); 
}
} // namespace eudaq
