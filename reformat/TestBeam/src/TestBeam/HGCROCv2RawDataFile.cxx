#include "TestBeam/HGCROCv2RawDataFile.h"

#include <iomanip>
#include <bitset>
#include <optional>

#include "Reformat/Utility/Mask.h"

namespace reformat {
namespace testbeam {

namespace debug {

struct hex {
  uint32_t word_;
  hex(uint32_t w) : word_{w} {}
};

}

inline std::ostream& operator<<(std::ostream& os, const debug::hex& h) {
  os << "0x" << std::setfill('0') << std::setw(8) << std::hex << h.word_ << std::dec;
  return os;
}

HGCROCv2RawDataFile::HGCROCv2RawDataFile(const framework::config::Parameters& ps)
    : reader_{ps.getParameter<std::string>("input_file")},
      reformat::RawDataFile(ps) {
  buffer_name_ = ps.getParameter<std::string>("name");
}

bool HGCROCv2RawDataFile::next(framework::Event& event) {
  static uint32_t head1, head2;

  /**
   * Rogue adds two header words to the begginning of each event
   *  headerA:
   *   [31:0] = Length of data block in bytes
   *  headerB
   *    31:24  = Channel ID
   *    23:16  = Frame error
   *    15:0   = Frame flags
   */
  reader_ >> head1 >> head2;
  int event_block_size = head1 / 4;

  std::cout << debug::hex(head1) << " : " << event_block_size << " words in event" << std::endl;
  std::cout << debug::hex(head2) << " : errors and flags " << std::endl;

  std::optional<uint32_t> old_rreq;
  std::vector<uint32_t> buffer;
  while (reader_ >> head1 >> head2) {
    /** Decode Bunch Header
     * We have a few words of header material before the actual data.
     * This header material is assumed to be encoded as in Table 3
     * of the DAQ specs.
     *
     * <name> (bits)
     *
     * VERSION (4) | FPGA_ID (8) | NLINKS (6) | 0 | LEN (12)
     * BX ID (12) | RREQ (10) | OR (10)
     * RID ok (1) | CDC ok (1) | LEN3 (6) |
     *  RID ok (1) | CDC ok (1) | LEN2 (6) |
     *  RID ok (1) | CDC ok (1) | LEN1 (6) |
     *  RID ok (1) | CDC ok (1) | LEN0 (6)
     * ... other listing of links ...
     */
    std::cout << debug::hex(head1) << " : ";
    uint32_t version =
        (head1 >> 28) & reformat::utility::mask<4>;
    std::cout << "version " << version << std::flush;
    uint32_t one{1};
    if (version != one)
      EXCEPTION_RAISE("VersMis", "HcalRawDecoder only knows version 1 of DAQ format.");
  
    uint32_t fpga = (head1 >> 12 + 2 + 6) & reformat::utility::mask<8>;
    uint32_t nlinks = (head1 >> 12 + 2) & reformat::utility::mask<6>;
    uint32_t len = head1 & reformat::utility::mask<12>;
  
    std::cout << ", fpga: " << fpga << ", nlinks: " << nlinks
              << ", len: " << len << std::endl;
  
    std::cout << debug::hex(head2) << " : ";
    uint32_t bx_id = (head2 >> 20);
    uint32_t rreq  = (head2 >> 10) & reformat::utility::mask<10>;
    uint32_t orbit = head2 & reformat::utility::mask<10>;

    std::cout << "bx_id: " << bx_id
              << ", rreq: " << rreq
              << ", orbit: " << orbit
              << std::endl;

    // now that we have gotten our two header words, go back
    reader_.seek<uint32_t>(-2, std::ios::cur);

    if (old_rreq and rreq != old_rreq) {
      // we are past the first bunch and hit a different
      // RREQ ==> don't copy this bunch into the buffer,
      // we are done with this event
      std::cout << "done with this event." << std::endl;
      break;
    } else if (!old_rreq) {
      // set old rreq on first one
      old_rreq = rreq;
    }
  
    std::cout << "copying data into buffer" << std::endl;
    // copy the data from this RREQ into the buffer
    std::size_t old_end = buffer.size();
    buffer.resize(old_end+len);
    for (int i{0}; i < len; i++) {
      reader_ >> buffer[old_end+i];
      if (!reader_) {
        EXCEPTION_RAISE("Malform","Expected a word when there wasn't one.");
      }
    }
  }

  // two trailing zeros?
  reader_.seek<uint32_t>(2, std::ios::cur);

  std::cout << "adding buffer to event bus" << std::endl;
  // broken out of loop when done because either
  //    reached EOF of archive OR
  //    new event ID has been read in
  // add the buffer to the event bus
  event.add(buffer_name_, buffer);

  std::cout << "returning " << std::boolalpha << !reader_ << std::endl;
  // we are done if reader is in an EoF state
  return !reader_;
}

} 
}  // namespace reformat

DECLARE_RAW_DATA_FILE(reformat::testbeam, HGCROCv2RawDataFile)
