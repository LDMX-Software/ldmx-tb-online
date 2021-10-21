#include "eudaq/HgcrocDataPacket.h"

//---< C++ >---//
#include <bitset>
#include <math.h>

namespace eudaq {

HgcrocDataPacket::HgcrocDataPacket(const eudaq::Event &event) {
  // Extract the data block from the event
  auto data{event.GetBlock(0x1)};

  // Extract the ROC subpacket
  createRocSubpackets(data);
}

void HgcrocDataPacket::createRocSubpackets(std::vector<uint8_t> data) {

  auto cindex{0};
  uint32_t header = data[cindex] | (data[cindex + 1] << 8) |
                    (data[cindex + 2] << 16) | (data[cindex + 3] << 24);
  // std::cout << "[ HgcrocFileReaderProducer ]:  word ( 1 ) : "
  //          << std::bitset<32>(header) << std::endl;
  n_links_ = (header >> 0xE) & 0x3F;
  fpga_id_ = (header >> 0x14) & 0xFF;
  format_version_ = (header >> 0x1C) & 0xF;

  cindex += 4;
  header = data[cindex] | (data[cindex + 1] << 8) | (data[cindex + 2] << 16) |
           (data[cindex + 3] << 24);
  orbit_counter_ = header & 0x3FF;
  rr_count_ = (header >> 0xA) & 0x3FF;
  bunch_id_ = (header >> 0x14) & 0xFFF;
  cindex += 4;

  std::vector<uint8_t> roc_lens;
  for (int info{0}; info < int(ceil(n_links_ / 4)); ++info) {
    header = data[cindex] | (data[cindex + 1] << 8) | (data[cindex + 2] << 16) |
             (data[cindex + 3] << 24);
    roc_lens.push_back(header & 0x3F);
    roc_lens.push_back((header >> 0x8) & 0x3F);
    roc_lens.push_back((header >> 0x10) & 0x3F);
    roc_lens.push_back((header >> 0x18) & 0x3F);
    cindex += 4;
  }

  cindex = header_size_ + ceil(n_links_ / 4.) * 4;
  for (auto &roc_len : roc_lens) {
    createRocSubpacket(cindex, roc_len * 4, data);
    cindex += roc_len * 4;
  }
}

void HgcrocDataPacket::createRocSubpacket(int cindex, int length,
                                          std::vector<uint8_t> packet) {
  RocSubpacket rs;
  cindex += 2; // Skip the readout map and CRC for now

  rs.roc_id = packet[cindex + 1] << 8 | packet[cindex];

  cindex += 6;
  uint32_t data = packet[cindex] | (packet[cindex + 1] << 8) |
                  (packet[cindex + 2] << 16) | (packet[cindex + 3] << 24);
  rs.wadd = (data >> 0x4) & 0xFF;
  rs.bunch_id = (data >> 0xD) & 0xFFF;

  cindex += 4;

  auto count{1};
  for (int i{cindex}; i < (cindex + length - 12); i += 4) {
    if (count == 19 || count == 20) {
      ++count;
      continue;
    }

    uint32_t channel_data = packet[i] | (packet[i + 1] << 8) |
                            (packet[i + 2] << 16) | (packet[i + 3] << 24);

    rs.adc.push_back(channel_data & 0x3FF);
    ++count;
  }

  subpackets_.push_back(rs);
}
} // namespace eudaq
