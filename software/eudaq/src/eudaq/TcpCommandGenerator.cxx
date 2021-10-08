#include "eudaq/TcpCommandGenerator.h"

//---< StdLib >---//
#include <stdio.h>

//---< rogue >---//
#include "rogue/interfaces/stream/Frame.h"
#include "rogue/interfaces/stream/FrameIterator.h"

namespace eudaq {

void TcpCommandGenerator::genFrame(uint8_t command) {

  auto frame{reqFrame(1, true)};

  // Update frame payload size
  frame->setPayload(1);

  // Get an iterator to the beginning of the frame
  auto it{frame->begin()};

  // Push the string
  toFrame(it, 1, &command);

  // Pass the frame to the TCP server
  sendFrame(frame);
}
} // namespace eudaq
