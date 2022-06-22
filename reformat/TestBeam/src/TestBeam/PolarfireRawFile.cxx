
#include <iomanip>
#include <optional>
#include <tuple>

#include "Reformat/RawDataFile.h"
#include "Reformat/Utility/Hex.h"
#include "Reformat/Utility/Mask.h"
#include "Reformat/Utility/Reader.h"

// uncomment to show decoding debug statements, 
// only helpful if decoding of event header is suspect
//#define SHOW_DECODING

namespace testbeam {

class PolarfireRawFile : public reformat::RawDataFile {
 public:
  PolarfireRawFile(const framework::config::Parameters& p);
  virtual std::optional<reformat::EventPacket> next() final override;
 private:
  /**
   * Actual event reading function, we separate it from next so that
   * we can implement skipping the first N spills
   */
  std::tuple<bool /*successful read*/,
             uint32_t /*local spill*/,
             uint32_t /*ticks*/,
             reformat::EventPacket /*packet WITHOUT TIMESTAMP*/
            > pop(); 
 private:
  long int spill_{-1};
  long int i_spill_{0};
  int spills_to_skip_{0};
};

PolarfireRawFile::PolarfireRawFile(const framework::config::Parameters& ps)
  : RawDataFile(ps) {
  spills_to_skip_ = ps.getParameter("spills_to_skip",spills_to_skip_);
}

std::optional<reformat::EventPacket> PolarfireRawFile::next() {
  static bool keep_current_spill{spills_to_skip_<=0};
  do {
    // pop does the actual unpacking and decoding of the event header
    auto [success, spill, ticks, ep] = pop();
    reformat_log(debug) << "popped event at spill " << spill << " with ticks " << ticks;
    // guard to return empty event packet upon end of file
    if (not success) return {};
    // check if new spill
    if (spill != spill_) {
      spill_ = spill;
      if (spills_to_skip_ > 0) {
        reformat_log(debug) << "Skipping spill " << spill;
        --spills_to_skip_;
        keep_current_spill = false;
      } else {
        reformat_log(debug) << "Keeping spill " << spill;
        keep_current_spill = true;
        ++i_spill_;
      }
    }

    if (keep_current_spill) {
      // not in skipping mode, return this event
      reformat::EventPacket::TimestampType ts;
      ts = i_spill_;
      ts <<= 32;
      ts += ticks;
      reformat_log(debug) << "Timestamp(i_spill = " << i_spill_ 
        << ", ticks = " << ticks << ") = " << ts;
      ep.setTimestamp(ts);
      return ep;
    }
  } while (not keep_current_spill);
  ldmx_log(error) << "escaped parsing loop when we were supposed to keep the current spill";
  return {};
}

std::tuple<bool,uint32_t, uint32_t, reformat::EventPacket> PolarfireRawFile::pop() {
  /// words for reading and decoding
  static uint32_t w;

  // special header words not counted in event length
  w=0;
  while (file_reader_ and w != 0xbeef2021 and w != 0xbeef2022) {
    file_reader_ >> w;
#ifdef SHOW_DECODING
    if (w == 0xbeef2021) {
      reformat_log(debug) << "Signal words imply version 1";
    } else if (w == 0xbeef2022) {
      reformat_log(debug) << "Signal words imply version 2";
    } else {
      reformat_log(debug) << "Extra header (inserted by rogue): " << reformat::utility::hex(w);
    }
#endif
  }

  reformat::EventPacket ep;

  if (!file_reader_ or file_reader_.eof()) 
    return std::make_tuple(false,0,0,ep);

  ep.append(w);

  /* whole event header word looks like
   *
   * VERSION (4) | FPGA ID (8) | NSAMPLES (4) | LEN (16)
   */
  long int i_event{0};
  file_reader_ >> w; i_event++;
  ep.append(w);

  uint32_t version  = (w >> 28) & reformat::utility::mask<4>;
  uint32_t fpga     = (w >> 20) & reformat::utility::mask<8>;
  uint32_t nsamples = (w >> 16) & reformat::utility::mask<4>;
  long int eventlen = w & reformat::utility::mask<16>;
  if (version == 2u) {
    // eventlen is 64-bit words in event,
    // need to multiply by 2 to get actual 32-bit event length
    eventlen *= 2;
    // and subtract off the special header word (beef202X) above
    eventlen -= 1;
  } else if (version == 1u) {
    EXCEPTION_RAISE("VersBad",
        "Version 1 of DAQ format does not have a event packet time stamp.");
  } else {
    EXCEPTION_RAISE("VersMis",
        "PolarfireRawFile only knows version 2 of DAQ format.");
  }
#ifdef SHOW_DECODING
  reformat_log(debug) << reformat::utility::hex(w)
    << " EventHeader(version = " << version
    << ", fpga = " << fpga
    << ", nsamples = " << nsamples
    << ", eventlen = " << eventlen
    << ")";
  reformat_log(debug) << "Sample Lengths: ";
#endif
  // sample counters
  int n_words{0};
  std::vector<uint32_t> length_per_sample(nsamples, 0);
  for (uint32_t i_sample{0}; i_sample < nsamples; i_sample++) {
    if (i_sample%2 == 0) {
      n_words++;
      file_reader_ >> w; i_event++;
      ep.append(w);
    }
    uint32_t shift_in_word = 16*(i_sample%2);
    length_per_sample[i_sample] = (w >> shift_in_word) & reformat::utility::mask<12>;
#ifdef SHOW_DECODING
    reformat_log(debug) << "len(" << i_sample << ") = " << length_per_sample[i_sample] << " ";
#endif
  }

  /**
   * For the time being, the number of sample lengths is fixed to make the
   * firmware for DMA readout simpler. This means we readout the leftover
   * dummy words to move the pointer on the reader.
   */
#ifdef SHOW_DECODING
  reformat_log(debug) << "Padding words to reach 8 total sample length words.";
#endif
  for (int i_word{n_words}; i_word < 8; i_word++) {
    file_reader_ >> w; i_event++;
    ep.append(w);
  }

  /**
   * extended event header in version 2
   */
  file_reader_ >> w; i_event++;
  ep.append(w);
  uint32_t spill = ((w >> 12) & 0xfff);
  uint32_t bunch = (w & 0xfff);
#ifdef SHOW_DECODING
  reformat_log(debug) << " " << reformat::utility::hex(w) 
    << " Spill: " << spill 
    << " Bunch: " << bunch;
#endif
  file_reader_ >> w; i_event++;
  ep.append(w);
  uint32_t ticks = w;
#ifdef SHOW_DECODING
  reformat_log(debug) << " " << reformat::utility::hex(w) 
    << " 5 MHz Ticks since Spill: " << w
    << " Time: " << w/5e6 << "s";
#endif
  file_reader_ >> w; i_event++;
  ep.append(w);
  uint32_t number = w;
#ifdef SHOW_DECODING
  reformat_log(debug) << " " << reformat::utility::hex(w) 
    << " Event Number: " << w;
#endif
  file_reader_ >> w; i_event++;
  ep.append(w);
  uint32_t run = (w & 0xFFF);
  uint32_t DD = (w>>23)&0x1F;
  uint32_t MM = (w>>28)&0xF;
  uint32_t hh = (w>>18)&0x1F;
  uint32_t mm = (w>>12)&0x3F;
#ifdef SHOW_DECODING
  reformat_log(debug) << " " << reformat::utility::hex(w) 
    << " Run: " << run << " DD-MM hh:mm "
    << DD << "-" << MM << " " << hh << ":" << mm;
#endif
  std::vector<uint32_t> words;
  // i_event words from eventlen already been read above
  if (file_reader_.read(words, eventlen-i_event)) ep.append(words);
  else {
    reformat_log(error) << "File closed before event completed";
    return std::make_tuple(false,spill,ticks,ep);
  }
  
  return std::make_tuple(true,spill,ticks,ep);
}

}  // namespace testbeam

DECLARE_RAW_DATA_FILE(testbeam::PolarfireRawFile);
