#include "emulators/TrigScintGenerator.h"

//---< StdLib >---//
#include <bitset>
#include <iostream>
#include <math.h>
#include <stdlib.h>

//---< rogue >---//
#include "rogue/interfaces/stream/Frame.h"
#include "rogue/interfaces/stream/FrameIterator.h"

//---< emulators >---//
#include "emulators/QieDataPacket.h"

namespace emulators {

void TrigScintGenerator::genFrame(uint32_t size) {
  rogue::interfaces::stream::FramePtr frame;
  rogue::interfaces::stream::FrameIterator it;

  // Calculate the total size of the frame
  uint16_t len = 124; // 31*32/8

  // Request a frame of the size above and get an iterator to the
  // beginning of the frame.
  frame = reqFrame(len, true);
  frame->setPayload(len);
  it = frame->begin();

  // Initialize QIE DataPacket
  auto qie{new QieDataPacket(event_number_)};

  // Set flags
  qie->setFlags(false, false, false, false);

  // Fill adc, tdc data (randomly)
  srand(time(NULL));
  for (int id{0}; id < 12; ++id) {
    for (int ts{0}; ts < 5; ++ts) {
      int adc{rand() % 256};
      int tdc{rand() % 50};
      qie->addQieData(id, ts, adc, tdc);
    }
  }

  // Generate data packet
  auto data = qie->formPacket();

  for (int i = 0; i < len; i++) {
    toFrame(it, 1, &data[i]);
  }
  ++event_number_;
 
  std::cout << qie << std::endl;

  // Build the QIE subpackets for each link
  sendFrame(frame);
}

} // namespace emulators
