#include "eudaq/HgcrocRawDecode.h"

//---< C++ >---//
#include <bitset>
#include <math.h>

namespace hcal {
namespace utility {
/**
 * Read out 32-bit words from a 8-bit buffer.
 */
class Reader {
  const std::vector<uint8_t> &buffer_;
  std::size_t i_word_;
  uint32_t next() {
    uint32_t w = buffer_.at(i_word_) | 
                 (buffer_.at(i_word_ + 1) << 8) |
                 (buffer_.at(i_word_ + 2) << 16) |
                 (buffer_.at(i_word_ + 3) << 24);
    i_word_ += 4;
    return w;
  }

 public:
  Reader(const std::vector<uint8_t>& b) : buffer_{b}, i_word_{0} {}
  operator bool() {
    return (i_word_ < buffer_.size());
  }
  Reader& operator>>(uint32_t& w) {
    if (*this)
      w = next();
    return *this;
  }
};  // Reader
/**
 * mask_backend
 *
 * The backend struct for the mask generation scheme.
 * Since we are using a template parameter rather than
 * a function argument, these masks are generated at
 * compile-time and so are equivalent to defining a set
 * of static const varaibles.
 */
template <short N>
struct mask_backend {
  /// value of mask
  static const uint64_t value = (1ul << N) - 1ul;
};

/**
 * Generate bit masks at compile time.
 *
 * The template input defines how many of
 * the lowest bits will be masked for.
 *
 * Use like:
 *
 *  i & mask<N>
 *
 * To mask for the lowest N bits in i.
 *
 * Maximum mask is 63 bits because we are using
 * a 64-bit wide integer and so we can't do
 *  1 << 64
 * without an error being thrown during compile-time.
 *
 * @tparam N number of lowest-order bits to mask
 */
template <short N>
constexpr uint64_t mask = mask_backend<N>::value;
} // utility
namespace debug {
struct hex {
  uint32_t word_;
  hex(uint32_t w) : word_{w} {}
};
} // debug

HgcrocSample::HgcrocSample(uint32_t w, int version) : word_(w), version_{version} {}

bool HgcrocSample::isTOTinProgress() const {
  return (1 & (word_ >> 31));
}

bool HgcrocSample::isTOTComplete() const {
  return (1 & (word_ >> 30));
}

int HgcrocSample::toa() const { 
  if (version_ == 2) {
    return secon();
  } else {
    return third(); 
  }
}

int HgcrocSample::tot() const {
  int meas = secon();
  if (version_ == 2) {
    meas = first();
  }

  if (meas > 512) meas = (meas - 512) * 8;
  return meas;
}

int HgcrocSample::adc_tm1() const { return first(); }

int HgcrocSample::adc_t() const {
  if (version_ == 2) {
    return third();
  }

  if (not isTOTComplete())
    return secon();  // running modes
  else
    return first();  // calibration mode
}

uint32_t HgcrocSample::raw() const { return word_; }

int HgcrocSample::first() const { return 0x3ff & (word_ >> 20); }

int HgcrocSample::secon() const { return 0x3ff & (word_ >> 10); }

int HgcrocSample::third() const { return 0x3ff & word_; }

ElectronicsLocation::ElectronicsLocation(unsigned int f, unsigned int l, unsigned int c)
  : fpga_{f}, link_{l}, inlink_channel_{c}, roc_{l / 2}, channel_{c - 36*(c>=36)} {}

unsigned int ElectronicsLocation::fpga() const { return fpga_; }
unsigned int ElectronicsLocation::link() const { return link_; }
unsigned int ElectronicsLocation::inlink_channel() const { return inlink_channel_; }
unsigned int ElectronicsLocation::roc() const { return roc_; }
unsigned int ElectronicsLocation::channel() const { return channel_; }

std::ostream& operator<<(std::ostream& os, const ElectronicsLocation& el) {
  return (os << "EL(" << el.fpga() << "," << el.roc() << "," << el.channel() << ")");
}

bool operator<(const ElectronicsLocation& lhs, const ElectronicsLocation& rhs) {
  if (lhs.fpga() < rhs.fpga()) return true;
  if (lhs.fpga() > rhs.fpga()) return false;
  // fpga is equal
  if (lhs.roc() < rhs.roc()) return true;
  if (lhs.roc() > rhs.roc()) return false;
  // fpga and link are equal
  return lhs.channel() < rhs.channel();
}

std::map<ElectronicsLocation,std::vector<HgcrocSample>>
decode(const std::vector<uint8_t>& binary_data) {
  /**
   * Static parameters depending on ROC version
   */
  static const unsigned int common_mode_channel = 19;
  /// words for reading and decoding
  static uint32_t head1, head2, w;

  // wrap byte buffer with reader for getting 32-bit words
  utility::Reader reader(binary_data);

  // special header words not counted in event length
  reader >> head1;
  if (head1 == 0x11111111) { reader >> head1; }
#ifdef DEBUG
  if (head1 == 0xbeef2021) {
    std::cout << "Signal words imply version 1" << std::endl;
  } else if (head1 == 0xbeef2022) {
    std::cout << "Signal words imply version 2" << std::endl;
  } else {
    std::cout << "Misunderstood signal word " << debug::hex(head1) << std::endl;
  }
#endif

  /**
   * Decode event header
   */
  long int eventlen;
  long int i_event{0};
  /* whole event header word looks like
   *
   * VERSION (4) | FPGA ID (8) | NSAMPLES (4) | LEN (16)
   */
  reader >> head1; i_event++;

  uint32_t version  = (head1 >> 28) & utility::mask<4>;
  uint32_t fpga     = (head1 >> 20) & utility::mask<8>;
  uint32_t nsamples = (head1 >> 16) & utility::mask<4>;
  eventlen = head1 & utility::mask<16>;
  if (version == 1u) {
    // eventlen is 32-bit words in event
    // do nothing here
  } else if (version == 2u) {
    // eventlen is 64-bit words in event,
    // need to multiply by 2 to get actual 32-bit event length
    eventlen *= 2;
    // and subtract off the special header word above
    eventlen -= 1;
  } else {
    throw std::runtime_error("HgcrocRawDecode only knows version 1 and 2 of DAQ format.");
  }
#ifdef DEBUG
  std::cout << debug::hex(head1)
    << " EventHeader(version = " << version
    << ", fpga = " << fpga
    << ", nsamples = " << nsamples
    << ", eventlen = " << eventlen
    << ")" << std::endl;
  std::cout << "Sample Lenghts: ";
#endif
  // sample counters
  int n_words{0};
  std::vector<uint32_t> length_per_sample(nsamples, 0);
  for (uint32_t i_sample{0}; i_sample < nsamples; i_sample++) {
    if (i_sample%2 == 0) {
      n_words++;
      reader >> w; i_event++;
    }
    uint32_t shift_in_word = 16*(i_sample%2);
    length_per_sample[i_sample] = (w >> shift_in_word) & utility::mask<12>;
#ifdef DEBUG
    std::cout << "len(" << i_sample << ") = " << length_per_sample[i_sample] << " ";
#endif
  }
#ifdef DEBUG
  std::cout << std::endl;
#endif

  if (version == 2) {
    /**
     * For the time being, the number of sample lengths is fixed to make the
     * firmware for DMA readout simpler. This means we readout the leftover
     * dummy words to move the pointer on the reader.
     */
#ifdef DEBUG
    std::cout << "Padding words to reach 8 total sample length words." << std::endl;
#endif
    for (int i_word{n_words}; i_word < 8; i_word++) {
      reader >> head1; i_event++;
#ifdef DEBUG
      std::cout << " " << debug::hex(head1);
#endif
    }
#ifdef DEBUG
    std::cout << std::endl;
#endif
  }

  /** 
   * Re-sort the data from grouped by bunch to by channel
   *
   * The readout chip streams the data off of it, so it doesn't
   * have time to re-group the signals across multiple bunches (samples)
   * by their channel ID. We need to do that here.
   */
  // fill map of **electronic** IDs to the digis that were read out
  std::map<ElectronicsLocation, std::vector<HgcrocSample>> eid_to_samples;
  std::size_t i_sample{0};
  while (i_event < eventlen) {
#ifdef DEBUG
    std::cout << "Decoding sample " << i_sample << " on word " << i_event << std::endl;
#endif
    reader >> head1 >> head2; i_event += 2;
    /** Decode Bunch Header
     * We have a few words of header material before the actual data.
     * This header material is assumed to be encoded as in Table 3
     * of the DAQ specs.
     *
     * <name> (bits)
     *
     * VERSION (4) | FPGA_ID (8) | NLINKS (6) | 00 | LEN (12)
     * BX ID (12) | RREQ (10) | OR (10)
     * RID ok (1) | CRC ok (1) | LEN3 (6) |
     *  RID ok (1) | CRC ok (1) | LEN2 (6) |
     *  RID ok (1) | CRC ok (1) | LEN1 (6) |
     *  RID ok (1) | CRC ok (1) | LEN0 (6)
     * ... other listing of links ...
     */
    //utility::CRC fpga_crc;
    //fpga_crc << head1;
#ifdef DEBUG
    std::cout << debug::hex(head1) << " : ";
#endif
    uint32_t hgcroc_version = (head1 >> 28) & utility::mask<4>;
#ifdef DEBUG
    std::cout << "hgcroc_version " << hgcroc_version << std::flush;
#endif
    uint32_t fpga = (head1 >> 20) & utility::mask<8>;
    uint32_t nlinks = (head1 >> 14) & utility::mask<6>;
    uint32_t len = head1 & utility::mask<12>;

#ifdef DEBUG
    std::cout << ", fpga: " << fpga << ", nlinks: " << nlinks << ", len: " << len << std::endl;
#endif
    //fpga_crc << head2;
#ifdef DEBUG
    std::cout << debug::hex(head2) << " : ";
#endif

    uint32_t bx_id = (head2 >> 20) & utility::mask<12>;
    uint32_t rreq = (head2 >> 10) & utility::mask<10>;
    uint32_t orbit = head2 & utility::mask<10>;

#ifdef DEBUG
    std::cout << "bx_id: " << bx_id << ", rreq: " << rreq << ", orbit: " << orbit << std::endl;
#endif

    std::vector<uint32_t> length_per_link(nlinks, 0);
    for (uint32_t i_link{0}; i_link < nlinks; i_link++) {
      if (i_link % 4 == 0) {
        i_event++; reader >> w;
        //fpga_crc << w;
#ifdef DEBUG
        std::cout << debug::hex(w) << " : Four Link Pack " << std::endl;
#endif
      }
      uint32_t shift_in_word = 8 * (i_link % 4);
      bool rid_ok = (w >> (shift_in_word + 7)) & utility::mask<1> == 1;
      bool cdc_ok = (w >> (shift_in_word + 6)) & utility::mask<1> == 1;
      length_per_link[i_link] =
          (w >> shift_in_word) & utility::mask<6>;
#ifdef DEBUG
      std::cout << "  Link " << i_link << " readout " << length_per_link.at(i_link) << " channels" << std::endl;
#endif
    }

    /** Decode Each Link in Sequence
     * Now we should be decoding each link serially
     * where each link was encoded as in Table 4 of
     * the DAQ specs
     *
     * ROC_ID (16) | CRC ok (1) | 0 (7) | RO Map (8)
     * RO Map (32)
     */

    for (uint32_t i_link{0}; i_link < nlinks; i_link++) {
      // move on from last word counting links or previous link
#ifdef DEBUG
      std::cout << "RO Link " << i_link << std::endl;
#endif
      //utility::CRC link_crc;
      i_event++; reader >> w;
      //fpga_crc << w;
      //link_crc << w;
      uint32_t roc_id = (w >> 16) & utility::mask<16>;
      bool crc_ok = (w >> 15) & utility::mask<1> == 1;
#ifdef DEBUG
      std::cout << debug::hex(w) << " : roc_id " << roc_id << ", crc_ok (v2 always false) " << std::boolalpha << crc_ok << std::endl;
#endif

      // get readout map from the last 8 bits of this word
      // and the entire next word
      std::bitset<40> ro_map = w & utility::mask<8>;
      ro_map <<= 32;
      i_event++; reader >> w;
      //fpga_crc << w;
      //link_crc << w;
      ro_map |= w;
#ifdef DEBUG
      std::cout << debug::hex(w) << " : lower 32 bits of RO map" << std::endl;
      std::cout << "Start looping through channels..." << std::endl;
#endif
      // loop through channels on this link,
      //  since some channels may have been suppressed because of low
      //  amplitude the channel ID is not the same as the index it
      //  is listed in.
      int channel_index{-1};
      for (uint32_t j{0}; j < length_per_link.at(i_link) - 2; j++) {
        // skip zero-suppressed channel IDs
        while (channel_index < 40 and not ro_map.test(channel_index)) {
          channel_index++;
        }

        // next word is this channel
        i_event++; reader >> w;
        //fpga_crc << w;
#ifdef DEBUG
        std::cout << debug::hex(w) << " " << channel_index;
#endif

        if (channel_index == 0) {
          /** Special "Header" Word from ROC
           *
           * version 3:
           * 0101 | BXID (12) | RREQ (6) | OR (3) | HE (3) | 0101
           *
           * version 2:
           * 10101010 | BXID (12) | WADD (9) | 1010
           */
#ifdef DEBUG
          std::cout << " : ROC Header";
#endif
          //link_crc << w;
          uint32_t bx_id = (w >> 16) & utility::mask<12>;
          uint32_t short_event = (w >> 10) & utility::mask<6>;
          uint32_t short_orbit = (w >> 7) & utility::mask<3>;
          uint32_t hamming_errs = (w >> 4) & utility::mask<3>;
        } else if (channel_index == common_mode_channel) {
          /** Common Mode Channels
           * 10 | 0000000000 | Common Mode ADC 0 (10) | Common Mode ADC 1 (10)
           */
          //link_crc << w;
#ifdef DEBUG
          std::cout << " : Common Mode";
#endif
        } else if (channel_index == 39) {
          // CRC checksum from ROC
          uint32_t crc = w;
#ifdef DEBUG
          std::cout << " : CRC checksum  : NOT CALCULATING CRC =? " << debug::hex(crc);
#endif
          /*
          if (link_crc.get() != crc) {
            EXCEPTION_RAISE("BadCRC",
                            "Our calculated link checksum doesn't match the "
                            "one from raw data.");
          }
          */
          
#ifdef DEBUG
          std::cout << " : CRC";
#endif
        } else {
          /// DAQ Channels

          //link_crc << w;
          /** Generate Packed Electronics ID
           *   Link Index         i_link
           *   In-link Channel ID channel_index - 1*(channel_index > common_mode_channel) - 1
           *   ROC ID             roc_id
           *   FPGA ID            fpga
           * are all available.
           * For now, we just generate a dummy mapping
           * using the link and channel indices.
           */

#ifdef DEBUG
          std::cout << " : DAQ Channel ";
          std::cout << fpga << " " << roc_id << " " << i_link << " " << channel_index << " ";
#endif
          /**
           * The subfields for the electronics ID infrastructure need to start
           * from 0 and count up. The valid range of the ID numbers is defined 
           * in the HcalElectronicsID class by the choice of the PackedIndex
           * template parameters. If any of the three IDs is out of this range,
           * the ID number will not be formed properly.
           */
          ElectronicsLocation eid{fpga, i_link, channel_index - 1*(channel_index > common_mode_channel) - 1};
          // copy data into EID->sample map
          eid_to_samples[eid].emplace_back(w);
        }  // type of channel
#ifdef DEBUG
        std::cout << std::endl;
#endif
      }  // loop over channels (j in Table 4)
#ifdef DEBUG
      std::cout << "done looping through channels" << std::endl;
#endif
    }  // loop over links

    // another CRC checksum from FPGA
    i_event++; reader >> w;
    uint32_t crc = w;
#ifdef DEBUG
    std::cout << "FPGA Checksum : NOT CALCULATED =? " << debug::hex(crc) << std::endl;
    std::cout << " N Sample Words : " << length_per_sample.at(i_sample) << std::endl;
#endif
    /* TODO
     *  fix calculation of FPGA checksum
     *  I can't figure out why it isn't matching, but there
     *  is definitely a word here where the FPGA checksum would be.
    if (fpga_crc.get() != crc) {
      EXCEPTION_RAISE(
          "BadCRC",
          "Our calculated FPGA checksum doesn't match the one read in.");
    }
    */
    // padding to reach 64-bit boundary in version 2
    if (version == 2u and length_per_sample.at(i_sample) % 2 == 1) {
      i_event++; reader >> head1;
#ifdef DEBUG
      std::cout << "Padding to reach 64-bit boundary: " << debug::hex(head1) << std::endl;
#endif
    }
    i_sample++;
  }

  if (version == 1u) {
    // special footer words
    reader >> head1 >> head2;
  }

  return eid_to_samples;
}

} // namespace hcal
