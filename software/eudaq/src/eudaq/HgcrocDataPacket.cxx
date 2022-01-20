#include "eudaq/HgcrocDataPacket.h"

//---< C++ >---//
#include <bitset>
#include <math.h>

namespace eudaq {

HgcrocDataPacket::HgcrocDataPacket(const eudaq::Event &event) {
  // Extract the data block from the event
  auto data{event.GetBlock(0x1)};

  // Extract the ROC subpacket
  createSamples(data);
}

void HgcrocDataPacket::createSamples(std::vector<uint8_t> data) {

  // Current index position in data vector
  auto cindex{0};

  // Skip the first two words of the header and the total length of the packet.
  // TODO: Add a check to make sure they are what we expect
  cindex += 10;

  // Extract the number of samples, fpga and format version
  uint16_t header = data[cindex] | (data[cindex + 1] << 8);
  // std::cout << "[ HgcrocFileReaderProducer ]:  word ( 3 ) : "
  //          << std::bitset<16>(header) << std::endl;

  n_samples_ = header & 0xF;
  fpga_id_ = (header >> 0x4) & 0xFF;
  format_version_ = (header >> 0xC) & 0xF;
  cindex += 2;

  // Use the number of samples to calcuate the number of 32-bit
  // words needed to store the sample lengths
  auto sample_header_count{int(ceil(n_samples_ / 2))};

  // Extract the number of words in each sample
  std::vector<uint16_t> sample_lens;
  uint32_t word;
  for (int ihead{0}; ihead < sample_header_count; ++ihead) {
    word = data[cindex] | (data[cindex + 1] << 8) | (data[cindex + 2] << 16) |
           (data[cindex + 3] << 24);
    // std::cout << "[ HgcrocFileReaderProducer ]:  " << std::bitset<32>(word)
    //          << std::endl;
    sample_lens.push_back(word & 0xFFF);
    sample_lens.push_back((word >> 0x10) & 0xFFF);
    cindex += 4;
  }

  for (auto &sample_len : sample_lens) {
    createSample(cindex, sample_len * 4, data);
  }
}

void HgcrocDataPacket::createSample(int cindex, int length,
                                    std::vector<uint8_t> data) {
  Sample sample;
  uint32_t word = data[cindex] | (data[cindex + 1] << 8) |
                  (data[cindex + 2] << 16) | (data[cindex + 3] << 24);
  // std::cout << "[ HgcrocFileReaderProducer ]: " << std::bitset<32>(word)
  //          << std::endl;

  sample.n_links = (word >> 0xE) & 0x3F;
  sample.fpga_id = (word >> 0x14) & 0xFF;
  sample.format_version = (word >> 0x1C) & 0xF;

  cindex += 4;

  word = data[cindex] | (data[cindex + 1] << 8) | (data[cindex + 2] << 16) |
         (data[cindex + 3] << 24);
  sample.orbit_counter = word & 0x3FF;
  sample.rr_count = (word >> 0xA) & 0x3FF;
  sample.bunch_id = (word >> 0x14) & 0xFFF;

  cindex += 4;

  std::vector<uint8_t> roc_lens;
  for (int info{0}; info < int(ceil(sample.n_links / 4.)); ++info) {
    word = data[cindex] | (data[cindex + 1] << 8) | (data[cindex + 2] << 16) |
           (data[cindex + 3] << 24);
    roc_lens.push_back(word & 0x3F);
    roc_lens.push_back((word >> 0x8) & 0x3F);
    roc_lens.push_back((word >> 0x10) & 0x3F);
    roc_lens.push_back((word >> 0x18) & 0x3F);
    cindex += 4;
  }

  // cindex = header_size_ + ceil(n_links_ / 4.) * 4;
  for (auto &roc_len : roc_lens) {
    sample.subpackets.push_back(createRocSubpacket(cindex, roc_len * 4, data));
    cindex += roc_len * 4;
  }
  samples_.push_back(sample);
}

RocSubpacket HgcrocDataPacket::createRocSubpacket(int cindex, int length,
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
  return rs;
}
} // namespace eudaq
