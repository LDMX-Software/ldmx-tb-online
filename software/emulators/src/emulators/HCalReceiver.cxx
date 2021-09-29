#include "emulators/HCalReceiver.h"

#include <bitset>
#include <iostream>

#include "rogue/interfaces/stream/Frame.h"
#include "rogue/interfaces/stream/FrameIterator.h"

namespace emulators {

void HCalReceiver::acceptFrame(
    std::shared_ptr<rogue::interfaces::stream::Frame> frame) {

  // Get an iterator to the data in the frame
  auto it{frame->begin()};

  // Check for errors. If errors are found, skip processing of the frame.
  if (frame->getError()) {
    ++rx_error_count_;
    return;
  }

  // Extract all info from the header
  uint32_t word;
  rogue::interfaces::stream::fromFrame(it, 4, &word);

  std::cout << "[ HCalReceiver ]: LEN: " << getField(word, 11, 0) << std::endl;
  int n_links{getField(word, 19, 14)}; 
  std::cout << "[ HCalReceiver ]: NLINKS: " << getField(word, 19, 14) << std::endl;
  std::cout << "[ HCalReceiver ]: FPGA_ID: " << getField(word, 27, 20) << std::endl;
  std::cout << "[ HCalReceiver ]: FMTVER: " << getField(word, 31, 28) << std::endl;

  rogue::interfaces::stream::fromFrame(it, 4, &word);
  std::cout << "[ HCalReceiver ]: OR: " << getField(word, 9, 0) << std::endl;
  std::cout << "[ HCalReceiver ]: RREQ: " << getField(word, 19, 10) << std::endl;
  std::cout << "[ HCalReceiver ]: BXID: " << getField(word, 31, 20) << std::endl;
 
  rogue::interfaces::stream::fromFrame(it, 4, &word);
  std::cout << "[ HCalReceiver ]: READOUT MAP: " << getField(word, 7, 0) << std::endl;
  std::cout << "[ HCalReceiver ]: ROC_ID: " << getField(word, 31, 16) << std::endl;

  rogue::interfaces::stream::fromFrame(it, 4, &word);
  std::cout << "[ HCalReceiver ]: READOUT MAP: " << getField(word, 31, 0) << std::endl;

  rogue::interfaces::stream::fromFrame(it, 4, &word);
  std::cout << "[ HCalReceiver ]: HE: " << getField(word, 6, 5) << std::endl;
  std::cout << "[ HCalReceiver ]: OR: " << getField(word, 8, 7) << std::endl;
  std::cout << "[ HCalReceiver ]: RREQ: " << getField(word, 14, 9) << std::endl;
  std::cout << "[ HCalReceiver ]: BXID: " << getField(word, 27, 15) << std::endl;
  
  it += 4; 
  for (int i{4}; i < n_links*42 - 1; i++) {
    rogue::interfaces::stream::fromFrame(it, 4, &word);
    std::cout << "[ HCalReceiver ]: f0f1[" << (i-4) << "]: " << getField(word, 31, 30) << std::endl;
    std::cout << "[ HCalReceiver ]: TOA[" << (i-4) << "]: " << getField(word, 9, 0) << std::endl;
    std::cout << "[ HCalReceiver ]: ADC[" << (i-4) << "]: " << getField(word, 19, 10) << std::endl;
    std::cout << "[ HCalReceiver ]: ADCt-1[" << (i-4) << "]: " << getField(word, 29, 20) << std::endl;
  }
  rogue::interfaces::stream::fromFrame(it, 4, &word);
  std::cout << "[ HCalReceiver ]: TAIL: " << getField(word, 31, 0) << std::endl;

  ++rx_count_;
  rx_bytes_ += frame->getPayload();
}

}; // namespace emulators
