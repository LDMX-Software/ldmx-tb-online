#include "emulators/TrigScintReceiver.h"

//---< StdLib >---//
#include <bitset>
#include <iostream>

//---< rogue >---//
#include "rogue/interfaces/stream/Frame.h"
#include "rogue/interfaces/stream/FrameIterator.h"

//---< emulators >---//
#include "emulators/QieDataPacket.h"

namespace emulators {

void TrigScintReceiver::acceptFrame(
    std::shared_ptr<rogue::interfaces::stream::Frame> frame) {

  // Get an iterator to the data in the frame
  auto it{frame->begin()};

  // Check for errors. If errors are found, skip processing of the frame.
  if (frame->getError()) {
    ++rx_error_count_;
    return;
  }

  // Get a placeholder to read the frame
  uint8_t word;
  int n_ts{5};
  int n_qie{12};

  rogue::interfaces::stream::fromFrame(it, 1, &word);
  int trig_id = 255 * word;
  rogue::interfaces::stream::fromFrame(it, 1, &word);
  trig_id += word;
  auto packet{new QieDataPacket(trig_id)};

  rogue::interfaces::stream::fromFrame(it, 1, &word);
  packet->setFlags(((word >> 3) & 1), ((word >> 2) & 1), ((word >> 1) & 1),
                   (word & 1));

  rogue::interfaces::stream::fromFrame(it, 1, &word);
  for (int ts = 0; ts < n_ts; ts++) {
    for (int qie = 0; qie < n_qie; qie++) {
      rogue::interfaces::stream::fromFrame(it, 1, &word);
      packet->setADC(ts, qie, word);
    }

    for (int qie = 0; qie < n_qie; qie++) {
      rogue::interfaces::stream::fromFrame(it, 1, &word);
      packet->setTDC(ts, qie, word);
    }
  }

  std::cout << packet << std::endl;

  ++rx_count_;
  rx_bytes_ += frame->getPayload();
}

}; // namespace emulators
