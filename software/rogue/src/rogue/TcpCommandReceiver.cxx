#include "rogue/TcpCommandReceiver.h"

//---< StdLib >---//
#include <bitset>
#include <iostream>

//---< ldmx-rogue >---//
#include "rogue/Commands.h"

//---< rogue >---//
#include "rogue/interfaces/stream/Frame.h"
#include "rogue/interfaces/stream/FrameIterator.h"

namespace rogue {

void TcpCommandReceiver::acceptFrame(
    std::shared_ptr<rogue::interfaces::stream::Frame> frame) {

  // Check for errors. If errors are found, skip processing of the frame.
  if (frame->getError()) {
    // ++rx_error_count_;
    return;
  }

  // Get an iterator to the data in the frame
  auto it{frame->begin()};

  // Extract the command
  uint8_t word;
  rogue::interfaces::stream::fromFrame(it, 1, &word);
  //std::cout << "[ TcpCommandReceiver ]: command : " << std::bitset<8>(word) << std::endl;

  if (word == rogue::commands::start) {
    start_run_ = true;
    stop_run_ = false;
  } else if (word == rogue::commands::stop) {
    start_run_ = false;
    stop_run_ = true;
  }
}
} // namespace rogue
