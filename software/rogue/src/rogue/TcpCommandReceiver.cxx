#include "rogue/TcpCommandReceiver.h"

//---< C++ StdLib >---//
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
  // std::cout << "[ TcpCommandReceiver ]: command : " << std::bitset<8>(word)
  // << std::endl;

  resetStates();
  if (word == rogue::commands::start) {
    start_run_ = true;
  } else if (word == rogue::commands::stop) {
    stop_run_ = true;
  } else if (word == rogue::commands::reset) {
    reset_ = true;
  } else if (word == rogue::commands::init) {
    init_ = true;
  } else if (word == rogue::commands::config) {
    config_ = true;
  }
}

void TcpCommandReceiver::resetStates() {
  start_run_ = false;
  stop_run_ = false;
  reset_ = false;
  init_ = false;
  config_ = false;
}
} // namespace rogue
