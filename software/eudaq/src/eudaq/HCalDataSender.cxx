#include "eudaq/HCalDataSender.h"

//---< C++ StdLib >---//
#include <bitset>
#include <cstring>
#include <iostream>

//---< rogue >---//
#include "rogue/interfaces/stream/Frame.h"
#include "rogue/interfaces/stream/FrameIterator.h"

namespace eudaq {
void HCalDataSender::sendEvent(
    std::shared_ptr<rogue::interfaces::stream::Frame> frame) {

  // Get an iterator to the data in the frame
  auto it{frame->begin()};

  // Extract the readout request from the header. This is assumed
  // to be a proxy for the trigger number.
  uint32_t word;
  it +=4;
  rogue::interfaces::stream::fromFrame(it, 4, &word);
  auto readout_req_{getField(word, 19, 10)};

  // Create an eudaq event to ship to the data collector. At this point,
  // the trigger number is set using the RREQ parameter in the HGCROC
  // data structure.
  auto event{eudaq::Event::MakeUnique("HCalTestBeamRaw")};
  event->SetTriggerN(readout_req_); 

  // Copy the data block from the rogue frame
  it = frame->begin();
  event->AddBlock(0x1, it.ptr(), frame->getPayload()); 

  //event->Print(std::cout); 
  // Send the event to the data collectors
  producer_->SendEvent(std::move(event)); 
}
} // namespace eudaq
