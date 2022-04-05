#include "eudaq/RogueDataSender.h"

//---< C++ StdLib >---//
#include <iostream>

//---< rogue >---//
#include "rogue/interfaces/stream/Frame.h"

namespace eudaq {
void RogueDataSender::acceptFrame(
    std::shared_ptr<rogue::interfaces::stream::Frame> frame) {

  // Check for errors. If errors are found, skip processing of the frame.
  if (frame->getError()) {
    ++rx_error_count_;
    return;
  }

  // Send the frame to the associated data collector
  sendEvent(frame);

  // Increase the RX counters
  ++rx_count_;
  rx_bytes_ += frame->getPayload();
}
} // namespace eudaq
