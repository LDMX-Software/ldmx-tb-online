#include "eudaq/RogueDataSender.h"

//#include <bitset>
#include <iostream>

#include "rogue/interfaces/stream/Frame.h"
#include "rogue/interfaces/stream/FrameIterator.h"

namespace eudaq {

void RogueDataSender::acceptFrame(
    std::shared_ptr<rogue::interfaces::stream::Frame> frame) {

  // Get an iterator to the data in the frame
  auto it{frame->begin()};

  // Check for errors. If errors are found, skip processing of the frame.
  if (frame->getError()) {
    ++rx_error_count_;
    return;
  }

  uint32_t word;

  auto event{eudaq::Event::MakeUnique("LDMXTestBeamRaw")};
  event->SetTriggerN(1); 
  event->Print(std::cout); 
  producer_->SendEvent(std::move(event)); 

  ++rx_count_;
  rx_bytes_ += frame->getPayload();

  std::cout << "Rx count: " << getRxCount() << " "
            << "Rx bytes: " << getRxBytes() << " "
            << "Rx errors: " << getRxErrors() << std::endl;
}

} // namespace eudaq
