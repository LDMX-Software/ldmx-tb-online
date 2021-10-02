#include "emulators/TrigScintGenerator.h"

#include <iostream>
#include <math.h>

#include "rogue/interfaces/stream/Frame.h"
#include "rogue/interfaces/stream/FrameIterator.h"
#include <bitset>

namespace emulators {

void TrigScintGenerator::genFrame(uint32_t size) {
  rogue::interfaces::stream::FramePtr frame;
  rogue::interfaces::stream::FrameIterator it;

  // Calculate the total size of the frame
  uint16_t len = header_size_ + trigger_id_size_ + qie_count_ * 2 + tail_size_;

  // Request a frame of the size above and get an iterator to the
  // beginning of the frame.
  frame = reqFrame(len, true);
  frame->setPayload(len);
  it = frame->begin();

  uint16_t header{format_version_};
  header |= (module_id_ << module_id_mask_);
  header |= (card_id_ << card_id_mask_);
  header |= (0x0 << 0x8); // Is CID unsynced
  header |= (0x0 << 0x9); // Is CID malformed
  header |= (0x0 << 0xA); // is CRC1 malformed
  header |= (qie_count_ << qie_count_mask_);
  std::cout << "[ TrigScintGenerator ]: Header [16:0]: "
            << std::bitset<16>(header) << std::endl;

  toFrame(it, 2, &header);

  ++event_number_;
  header = event_number_;
  std::cout << "[ TrigScintGenerator ]: Header [31:17]: "
            << std::bitset<16>(header) << std::endl;
  toFrame(it, 2, &header);

  // Build the QIE subpackets for each link
  std::vector<uint16_t> subpackets{buildQIESubpackets(int(qie_count_))};
  std::cout << subpackets.size();
  int j{0};
  for (auto &subpacket : subpackets) {
    std::cout << "[ TrigScintGenerator ]: ROC subpacket j = " << j << " : "
              << std::bitset<16>(subpacket) << std::endl;
    toFrame(it, 2, &subpacket);
    ++j;
  }

  // The CRC tail -- Not currently used
  uint32_t tail{0};
  std::cout << "[ TrigScintGenerator ]: Tail: " << std::bitset<16>(tail)
            << std::endl;
  toFrame(it, 2, &tail);

  sendFrame(frame);
}

std::vector<uint16_t> TrigScintGenerator::buildQIESubpackets(int qie_count) {

  uint8_t fiber_id = 0x1;
  uint8_t channel_id = 0;
  std::vector<uint16_t> packets(qie_count * 2, 0x0);

  uint16_t adc{100};
  uint16_t adc_exp{2};
  uint16_t tdc{1000};

  int j{0};
  for (int i{0}; i < qie_count; ++i) {
    packets[j] = fiber_id;
    packets[j] |= (channel_id << 0x1);

    packets[j + 1] |= adc;
    packets[j + 1] |= (adc_exp << adc_exp_mask_);
    packets[j + 1] |= (tdc << tdc_mask_);

    ++channel_id;
    adc += 100;
    tdc += 1000;
    j += 2;
  }
  return packets;
}

} // namespace emulators
