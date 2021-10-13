#include "emulators/TrigScintGenerator.h"

#include <iostream>
#include <math.h>

#include "rogue/interfaces/stream/Frame.h"
#include "rogue/interfaces/stream/FrameIterator.h"
#include <bitset>

#include "emulators/QIE_DataPacket.h"
#include <stdlib.h>     /* srand, rand */

namespace emulators {

void TrigScintGenerator::genFrame(uint32_t size) {
  rogue::interfaces::stream::FramePtr frame;
  rogue::interfaces::stream::FrameIterator it;printf("Generator L17\n");

  // Calculate the total size of the frame
  uint16_t len = 124;		// 31*32/8

  // Request a frame of the size above and get an iterator to the
  // beginning of the frame.
  frame = reqFrame(len, true);
  frame->setPayload(len);
  it = frame->begin();

  // Initialize QIE DataPacket
  QIE_DataPacket* qie = new QIE_DataPacket(event_number_);

  // Set flags
  qie->SetFlags(false,false,false,false);

  // Fill adc, tdc data (randomly)
  srand (time(NULL));
  for(int id=0;id<12;id++){
    for(int ts=0;ts<5;ts++){
      int adc = rand() % 256;
      int tdc = rand() % 50;
      qie->AddQIEData(id,ts,adc,tdc);
    }
  }

  // Generate data packet
  auto data = qie->FormPacket();

  for(int i=0;i<len;i++){
    toFrame(it,1,&data[i]);
  }
  ++event_number_;
  qie->PrintQIEInfo(data);

  // Build the QIE subpackets for each link
  sendFrame(frame);
}

} // namespace emulators
