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
  rogue::interfaces::stream::FrameIterator it;

  // // Calculate the total size of the frame
  // uint16_t len = header_size_ + trigger_id_size_ + qie_count_ * 2 + tail_size_;

  // // Request a frame of the size above and get an iterator to the
  // // beginning of the frame.
  // frame = reqFrame(len, true);
  // frame->setPayload(len);
  // it = frame->begin();

  // uint16_t header{format_version_};
  // header |= (module_id_ << module_id_mask_);
  // header |= (card_id_ << card_id_mask_);
  // header |= (0x0 << 0x8); // Is CID unsynced
  // header |= (0x0 << 0x9); // Is CID malformed
  // header |= (0x0 << 0xA); // is CRC1 malformed
  // header |= (qie_count_ << qie_count_mask_);
  // std::cout << "[ TrigScintGenerator ]: Header [16:0]: "
  //           << std::bitset<16>(header) << std::endl;

  // toFrame(it, 2, &header);

  // ++event_number_;
  // header = event_number_;
  // std::cout << "[ TrigScintGenerator ]: Header [31:17]: "
  //           << std::bitset<16>(header) << std::endl;
  // toFrame(it, 2, &header);

  // // Build the QIE subpackets for each link
  // std::vector<uint16_t> subpackets{buildQIESubpackets(int(qie_count_))};
  // std::cout << subpackets.size();
  // int j{0};
  // for (auto &subpacket : subpackets) {
  //   std::cout << "[ TrigScintGenerator ]: ROC subpacket j = " << j << " : "
  //             << std::bitset<16>(subpacket) << std::endl;
  //   toFrame(it, 2, &subpacket);
  //   ++j;

  // Calculate the total size of the frame
  uint16_t len = 124;		// 31*32/8

  // Request a frame of the size above and get an iterator to the
  // beginning of the frame.
  frame = reqFrame(len, true);
  frame->setPayload(len);
  it = frame->begin();

  // Initialize QIE DataPacket
  // auto qie = new QIE_DataPacket(event_number_);
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
