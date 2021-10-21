#include "rogue/TrigScintReceiver.h"

//---< C++ >---//
#include <iostream>

//---< rogue >---//
#include "rogue/interfaces/stream/Frame.h"
#include "rogue/interfaces/stream/FrameIterator.h"

namespace eudaq {
void TrigScintReceiver::acceptFrame(
    std::shared_ptr<rogue::interfaces::stream::Frame> frame) {

  // Check for errors. If errors are found, skip processing of the frame.
  if (frame->getError()) {
    return;
  }

  // Get an iterator to the data in the frame
  auto it{frame->begin()};

  // Get the size of the frame and create a string buffer
  // to copy the data into.
  auto size{frame->getPayload()};
  std::string buffer; 
  buffer.reserve(size); 

  // Copy the buffer
  rogue::interfaces::stream::fromFrame(it, size, &buffer[0]); 
  //std::copy(frame->begin(), frame->end(), buffer);

  // convert to a string

  //std::string result = convertToString(buffer, size);

  std::cout << buffer << std::endl;
  // Print the values in the first 10 locations
  // for (int x=0; x < size; x++) {
  // printf("Entry: %d\n", *it);
  // it++;
  //}
}
} // namespace eudaq
