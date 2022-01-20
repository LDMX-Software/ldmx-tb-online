#include "emulators/HCalGenerator.h"

#include <iostream>
#include <math.h>

#include "rogue/interfaces/stream/Frame.h"
#include "rogue/interfaces/stream/FrameIterator.h"
#include <bitset>

namespace emulators {

void HCalGenerator::genFrame(uint32_t size) {
  rogue::interfaces::stream::FramePtr frame;
  rogue::interfaces::stream::FrameIterator it;

  // Calculate the total size of the frame
  uint16_t len = header_size_ + ceil(n_links_ / 4.) * 4 +
                 n_links_ * roc_subpacket_size_ + tail_size_;

  // Request a frame of the size above and get an iterator to the
  // beginning of the frame.
  frame = reqFrame(len, true);
  frame->setPayload(len);
  it = frame->begin();

  uint32_t header{len};
  header |= (n_links_ << n_links_mask_);
  header |= (fpga_id_ << fpga_id_mask_);
  header |= (format_version_ << format_version_mask_);

  // std::cout << "[ HCalGenerator ]: Header [31:0]: " <<
  // std::bitset<32>(header)
  //          << std::endl;

  toFrame(it, 4, &header);

  header = orbit_counter_;
  header |= (rr_count_ << rr_mask_);
  header |= (bunch_id_ << bunch_id_mask_);

  // std::cout << "[ HCalGenerator ]: Header [63:32]: " <<
  // std::bitset<32>(header)
  //          << std::endl;

  toFrame(it, 4, &header);

  // Build the ROC info header
  std::vector<uint32_t> roc_info{buildRocInfo(n_links_)};
  // int k{0};
  for (auto &info : roc_info) {
    // std::cout << "[ HCalGenerator ]: ROC info ( " << k << " ) : "
    //          << std::bitset<32>(info) << std::endl;
    toFrame(it, 4, &info);
    //++k;
  }

  // Build the ROC subpackets for each link
  std::vector<uint32_t> subpackets{
      buildRocSubpackets(int(n_links_), orbit_counter_, bunch_id_)};
  // int j{0};
  for (auto &subpacket : subpackets) {
    // std::cout << "[ HCalGenerator ]: ROC subpacket j = " << j << " : "
    //          << std::bitset<32>(subpacket) << std::endl;
    toFrame(it, 4, &subpacket);
    //++j;
  }

  // Increment the bunch ID
  ++bunch_id_;
  ++rr_count_;

  // Assume 10 bunches per train
  if ((bunch_id_ + 1) % 10 == 0)
    orbit_counter_ += 1;

  // The CRC-32 tail -- Not currently used
  uint32_t tail{0};
  // std::cout << "[ HCalGenerator ]: Tail: " << std::bitset<32>(tail)
  //          << std::endl;
  toFrame(it, 4, &tail);

  sendFrame(frame);
}

std::vector<uint32_t> HCalGenerator::buildRocInfo(int n_links) {

  std::vector<uint32_t> roc_info(int(ceil(n_links / 4.)), 0x0);
  std::vector<uint8_t> roc_info_mask = {0x0, 0x8, 0x10, 0x18};
  std::vector<uint8_t> crc_ok_mask = {0x6, 0xE, 0x16, 0x1E};
  std::vector<uint8_t> rid_ok_mask = {0x7, 0xF, 0x17, 0x1F};
  for (int ilink{0}; ilink < n_links; ++ilink) {
    auto index{int(floor(ilink / 4.))};
    auto mask_index{ilink % 4};
    roc_info[index] |= (roc_subpacket_size_ << roc_info_mask[mask_index]);
    roc_info[index] |= (0x1 << crc_ok_mask[mask_index]);
    roc_info[index] |= (0x1 << rid_ok_mask[mask_index]);
  }

  return roc_info;
}

std::vector<uint32_t> HCalGenerator::buildRocSubpackets(int n_links,
                                                        uint16_t orbit_counter,
                                                        uint16_t bunch_id) {
  std::vector<uint32_t> packet(int(n_links * 42), 0x0);

  // Build the header of the subpacket
  packet[0] = 0xFF; // Readout map
  packet[0] |= (0x1 << crc_ok_mask_);
  packet[0] |= (0x1 << roc_id_mask_);

  // Set the rest of the readout map to all 1's
  packet[1] = -1;

  packet[2] = 0x5;
  packet[2] |= (0x0 << hamming_errors_mask_);
  packet[2] |= (orbit_counter << roc_orbit_counter_mask_);
  packet[2] |= (0x1 << roc_rr_mask_);
  packet[2] |= (bunch_id << roc_bunch_id_mask_);
  packet[2] |= (0x5 << 0x1C);

  packet[3] = 0x3FF;
  packet[3] |= (0x0 << adc_mask_);
  packet[3] |= (0x0 << adc_t1_mask_);
  packet[3] |= (0x2 << channel_valid_mask_);

  uint16_t adc_t1{100};
  uint16_t adc{200};
  uint16_t toa{1};
  for (int i{4}; i < packet.size() - 1; ++i) {
    packet[i] = toa;
    packet[i] |= (adc << adc_mask_);
    packet[i] |= (adc_t1 << adc_t1_mask_);
    packet[i] |= (0x0 << channel_valid_mask_);

    adc_t1++;
    adc++;
    toa++;
  }

  return packet;
}

} // namespace emulators
