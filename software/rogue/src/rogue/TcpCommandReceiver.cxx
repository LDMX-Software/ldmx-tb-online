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

  resetStates();
  
  // Get an iterator to the data in the frame
  auto it{frame->begin()};

  // Get the payload size. If it's bigger than a single byte, assume it's a
  // string buffer.
  auto size{frame->getPayload()};
  if (size != 1) {
    // Get the payload size and reserve the memory
    buffer_.reserve(size);

    // Copy the buffer
    rogue::interfaces::stream::fromFrame(it, size, &buffer_[0]);

    // Set the command flag indicating a command has been received
    command_ = true;

    return;
  }

  // Extract the first word and determine what run state we are in
  uint8_t word;
  rogue::interfaces::stream::fromFrame(it, 1, &word);
  // std::cout << "[ TcpCommandReceiver ]: command : " << std::bitset<8>(word)
  // << std::endl;

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
  command_ = false;
  buffer_.clear(); 
}
} // namespace rogue
