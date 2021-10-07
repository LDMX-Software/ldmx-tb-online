#include "eudaq/TcpCommandGenerator.h"

//---< StdLib >---//
#include <stdio.h>

//---< rogue >---//
#include "rogue/interfaces/stream/Frame.h"
#include "rogue/interfaces/stream/FrameIterator.h"

namespace eudaq {

void TcpCommandGenerator::genFrame(char *command) {

  // Request a frame to store the command in
  size_t size{sizeof(command)};
  auto frame{reqFrame(size, true)};

  // Update frame payload size
  frame->setPayload(size);

  // Get an iterator to the beginning of the frame
  auto it{frame->begin()};

  // Push the string
  toFrame(it, size, &command);

  // Pass the frame to the TCP server
  sendFrame(frame);
}
} // namespace eudaq
