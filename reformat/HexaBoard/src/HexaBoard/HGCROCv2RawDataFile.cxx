#include "HexaBoard/HGCROCv2RawDataFile.h"

#include <algorithm>
#include <iomanip>

#include "Reformat/Utility/CRC.h"
#include "Reformat/Utility/Mask.h"

/**
 * The number of readout channels is defined by the hardware
 * construction of a HGC ROC as well as our DAQ readout specs.
 *
 * We have defined a method for keeping track of zero suppression
 * along the DAQ path using a bit-map. Since we aren't using
 * zero suppression currently in the hexactrl-sw DAQ, this ends
 * up being a map of all ones.
 *
 * We have 38 channels that are always read out plus
 *  a header word at the top and a CRC checksum at the bottom
 */
#define N_READOUT_CHANNELS 40

namespace reformat {
namespace hexaboard {

using utility::CRC;
using utility::mask;

bool HGCROCv2RawDataFile::next(framework::Event& event) {
  static bool first_sample{true};

  std::vector<uint32_t> buffer;
  int current_event{the_sample_.event()};

  if (first_sample) {
    // on the very first sample in the run,
    // we need to get the current event from the archive
    try {
      input_archive_ >> the_sample_;
    } catch (boost::archive::archive_exception& e) {
      EXCEPTION_RAISE(
          "BadInput",
          std::string("Boost.Arhcive exception reached on first sample. ") +
              e.what());
    }
    current_event = the_sample_.event();
    first_sample = false;
  }

  bool done{false};
  // loop until we change event numbers OR
  // Boost throws an archive exception signaling that
  // we have reached End of File
  while (true) {
    the_sample_.put(buffer);
    try {
      input_archive_ >> the_sample_;
    } catch (boost::archive::archive_exception&) {
      // boost archive exception thrown during reading
      // assume we have reached EOF
      done = true;
      break;
    }
    if (the_sample_.event() != current_event) break;
  }

  // broken out of loop when done because either
  //    reached EOF of archive OR
  //    new event ID has been read in
  // add the buffer to the event bus
  event.add(buffer_name_, buffer);
  return done;
}

void HGCROCv2RawDataFile::Sample::stream(std::ostream& out) const {
  out << "event = " << std::dec << m_event << " "
      << "chip = " << std::dec << m_chip << std::endl;

  out << "first half :";
  for (auto d : m_data0)
    out << "  " << std::hex << std::setfill('0') << std::setw(8) << d;
  out << std::endl;
  out << "second half :";
  for (auto d : m_data1)
    out << "  " << std::hex << std::setfill('0') << std::setw(8) << d;
  out << std::dec << std::endl;
}

void HGCROCv2RawDataFile::Sample::put(std::vector<uint32_t>& buffer) const {
  // arbitrary ID values that are meaningless right now
  static const int fpga{42};
  static const int orbit{13};
  static const int roc{7};
  // current word we are using to help create buffer
  static uint64_t word;

  CRC fpga_crc;

  /** Insert new header here
   * In the DAQ specs, the header is separated into 32-bit words,
   * so we do the same here. Below, I've copied down the structure
   * that I've implemented for ease of reference.
   * The numbers in parentheses are the number of bits.
   *
   * VERSION (4) | FPGA (8) | NLINKS(6) | 0 | TOTAL_RO(12)
   * BX ID (12) | RREQ (10) | OR (10)
   *
   * We need to do some fancy footwork to extract the BX ID from the
   * ROC data because it is not decoded in hexactrl-sw by default.
   */
  word = (1 & mask<4>) + // 4 bit version number
         ((fpga & mask<8>) << 4) +
         ((2 & mask<6>) << 12) +
         (0 << 18) +
         ((2 * N_READOUT_CHANNELS & mask<12>) << 19);
  buffer.push_back(word);
  fpga_crc(word);

  unsigned int bx_id{0};
  try {
    unsigned int bx_id_0{(m_data0.at(0) >> 12) & 0xfff};
    unsigned int bx_id_1{(m_data1.at(0) >> 12) & 0xfff};
    if (bx_id_0 == bx_id_1) bx_id = bx_id_0;

    /** Don't worry about this for now...
    if (debug_)
      std::cout << "BX IDs: " << bx_id_0 << " " << bx_id_1 << std::endl;
    else
      throw std::runtime_error("Received two different BX IDs at once.");
      */
  } catch (std::out_of_range&) {
    EXCEPTION_RAISE("BadROCData", "Received ROC data without a header.");
  }

  /*
  if (bx_id == 0)
    throw std::runtime_error("Unable to deduce BX ID.");
    */

  word = (bx_id & mask<12>) +
         ((m_event & mask<10>) << 12) +
         ((orbit & mask<10>) << 22);
  buffer.push_back(word);
  fpga_crc(word);

  /** Insert link counters
   * We only have two links in this ROC,
   * so the last half of this word will be zeroes.
   *
   * And then, we aren't doing any zero-suppression,
   * so the number of channels readout will always
   * be the same for both links.
   * The numbers in parentheses are the number of bits.
   *
   * RID ok (1) | CRC ok (1) | LEN0 (6)
   *  | RID ok (1) | CRC ok (1) | LEN1 (6)
   *  | sixteen zeros
   */
  word = (1     ) + (1 << 2) + ((N_READOUT_CHANNELS & mask<6>) << 2) +
         (1 << 8) + (1 << 9) + ((N_READOUT_CHANNELS & mask<6>) << 10) +
         (0 << 16); // zero-pad the word when done with links
  buffer.push_back(word);
  fpga_crc(word);

  /** Go through both of our links
   * In our case, the two "half"s of the ROC are our two links.
   *
   * The hexactrl-sw does not insert the header for each link
   * because there is no zero-suppression or other ROCs to worry about,
   * so we need to insert an additional header here.
   *
   * ROC ID (16) | CRC ok (1) | 00000 | Readout Map [39:32]
   *
   * The numbers in parentheses are the number of bits.
   */
  for (const auto& link_data : {m_data0, m_data1}) {
    CRC link_crc;
    word = (roc & mask<16>) +
           (1 << 16) +
           (0 << 17) +
           (mask<8> << 22);
    buffer.push_back(word);
    fpga_crc(word);
    link_crc(word);
    // rest of readout map (everything is being readout)
    buffer.push_back(0xFFFFFFFF);
    fpga_crc(0xFFFFFFFF);
    link_crc(0xFFFFFFFF);
    /** header word from ROC
     * 0101 | BX ID (12) | RREQ (6) | OR (3) | HE (3) | 0101
     */
    word =
        (0b0101) +
        ((bx_id & mask<12>) << 4) +
        ((m_event & mask<6>) << 16) +
        ((orbit & mask<3>) << 22) +
        ((0 & mask<3>) << 25) +
        (0b0101 << 28);
    buffer.push_back(word);
    fpga_crc(word);
    link_crc(word);

    // copy in _data_ words from hexactrl-sw
    //  (we already decoded the header to get the BX ID above)
    //  and we drop the four commas that are used by the ROC
    //  to signal the end of a group
    for (auto w_it{link_data.begin() + 1}; w_it != link_data.end() - 4;
         w_it++) {
      buffer.push_back(*w_it);
      fpga_crc(*w_it);
      link_crc(*w_it);
    }

    // ROC CRC Checksum
    buffer.push_back(link_crc.get());
    fpga_crc(link_crc.get());
  }

  // CRC Checksum computed by FPGA
  buffer.push_back(fpga_crc.get());
}

}  // namespace hexaboard
}  // namespace reformat

DECLARE_RAW_DATA_FILE(reformat::hexaboard, HGCROCv2RawDataFile)
