
#include <bitset>
#include <iomanip>
#include <optional>

#include "Reformat/RawDataFile.h"
#include "Reformat/Utility/Hex.h"
#include "Reformat/Utility/Mask.h"
#include "Reformat/Utility/Reader.h"

namespace testbeam {

class PolarfireRawFile : public reformat::RawDataFile {
 public:
  PolarfireRawFile(const framework::config::Parameters& p);
  virtual std::optional<reformat::EventPacket> next() final override;
 private:
  long int spill_{-1};
  long int i_spill_{0};
};

PolarfireRawFile::PolarfireRawFile(const framework::config::Parameters& ps)
  : RawDataFile(ps) {}

std::optional<reformat::EventPacket> PolarfireRawFile::next() {
  /// words for reading and decoding
  static uint32_t w;

  // special header words not counted in event length
  w=0;
  while (file_reader_ and w != 0xbeef2021 and w != 0xbeef2022) {
    file_reader_ >> w;
    if (w == 0xbeef2021) {
      reformat_log(debug) << "Signal words imply version 1";
    } else if (w == 0xbeef2022) {
      reformat_log(debug) << "Signal words imply version 2";
    } else {
      reformat_log(debug) << "Extra header (inserted by rogue): " << reformat::utility::hex(w);
    }
  }
  if (!file_reader_ or file_reader_.eof()) return {};

  reformat::EventPacket ep;
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
  reformat_log(debug) << reformat::utility::hex(w)
    << " EventHeader(version = " << version
    << ", fpga = " << fpga
    << ", nsamples = " << nsamples
    << ", eventlen = " << eventlen
    << ")";
  reformat_log(debug) << "Sample Lengths: ";
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
    reformat_log(debug) << "len(" << i_sample << ") = " << length_per_sample[i_sample] << " ";
  }

  /**
   * For the time being, the number of sample lengths is fixed to make the
   * firmware for DMA readout simpler. This means we readout the leftover
   * dummy words to move the pointer on the reader.
   */
  reformat_log(debug) << "Padding words to reach 8 total sample length words.";
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
  reformat_log(debug) << " " << reformat::utility::hex(w) 
    << " Spill: " << spill 
    << " Bunch: " << bunch;
  file_reader_ >> w; i_event++;
  ep.append(w);
  uint32_t ticks = w;
  reformat_log(debug) << " " << reformat::utility::hex(w) 
    << " 5 MHz Ticks since Spill: " << w
    << " Time: " << w/5e6 << "s";
  file_reader_ >> w; i_event++;
  ep.append(w);
  uint32_t number = w;
  reformat_log(debug) << " " << reformat::utility::hex(w) 
    << " Event Number: " << w;
  file_reader_ >> w; i_event++;
  ep.append(w);
  uint32_t run = (w & 0xFFF);
  uint32_t DD = (w>>23)&0x1F;
  uint32_t MM = (w>>28)&0xF;
  uint32_t hh = (w>>18)&0x1F;
  uint32_t mm = (w>>12)&0x3F;
  reformat_log(debug) << " " << reformat::utility::hex(w) 
    << " Run: " << run << " DD-MM hh:mm "
    << DD << "-" << MM << " " << hh << ":" << mm;

  /**
   * Convert our event header information into global timestamp
   */
  if (spill != spill_) {
    spill_ = spill;
    i_spill_++;
  }
  reformat::EventPacket::TimestampType ts;
  ts = i_spill_;
  ts <<= 32;
  ts += ticks;
  reformat_log(debug) << " Timestamp(i_spill = " << i_spill_ << ", ticks = " << ticks << ") = " << ts;
  ep.setTimestamp(ts);

  std::vector<uint32_t> words;
  // i_event words from eventlen already been read above
  if (file_reader_.read(words, eventlen-i_event)) ep.append(words);
  else {
    reformat_log(error) << "File closed before event completed";
    return {};
  }

  return ep;
}

}  // namespace testbeam

DECLARE_RAW_DATA_FILE(testbeam::PolarfireRawFile);
