#include "emulators/TrigScint_SlowControlReceiver.h"

//---< StdLib >---//
#include <bitset>
#include <iostream>

#include "rogue/interfaces/stream/Frame.h"
#include "rogue/interfaces/stream/FrameIterator.h"

namespace emulators {

void TrigScint_SlowControlReceiver::acceptFrame(
    std::shared_ptr<rogue::interfaces::stream::Frame> frame) {

  // Get an iterator to the data in the frame
  auto it{frame->begin()};

  // Check for errors. If errors are found, skip processing of the frame.
  if (frame->getError()) {
    ++rx_error_count_;
    return;
  }

  // Extract all info from the header
  uint16_t word;
  rogue::interfaces::stream::fromFrame(it, 2, &word);

  std::cout << "[ TrigScint_SlowControlReceiver ]: FMTVER: " << getField(word, 3, 0) << std::endl;
  std::cout << "[ TrigScint_SlowControlReceiver ]: MODULE ID: " << getField(word, 5, 4) << std::endl;
  std::cout << "[ TrigScint_SlowControlReceiver ]: CARD_ID: " << getField(word, 7, 6) << std::endl;
  std::cout << "[ TrigScint_SlowControlReceiver ]: CID unsynced: " << getField(word, 8, 8) << std::endl;
  std::cout << "[ TrigScint_SlowControlReceiver ]: CID malformed: " << getField(word, 9, 9) << std::endl;
  std::cout << "[ TrigScint_SlowControlReceiver ]: CRC1 malformed: " << getField(word, 10, 10) << std::endl;
  std::cout << "[ TrigScint_SlowControlReceiver ]: QIE COUNT: " << getField(word, 15, 11) << std::endl;

  rogue::interfaces::stream::fromFrame(it, 2, &word);
  std::cout << "[ TrigScint_SlowControlReceiver ]: TRIG COUNT: " << word << std::endl;

  ++rx_count_;
  rx_bytes_ += frame->getPayload();
}

}; // namespace emulators
