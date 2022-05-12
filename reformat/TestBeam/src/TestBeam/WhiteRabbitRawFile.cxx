
#include "Reformat/RawDataFile.h"

namespace testbeam {

class WhiteRabbitEventPacket {
  std::vector<uint32_t> data_;
 public:
  reformat::utility::Reader& read(reformat::utility::Reader& r) {
    return r.read(data_, 7);
  }
  const std::vector<uint32_t>& data() const {
    return data_;
  }
  uint64_t timestamp() const {
    return (
        data_.at(4)*1e9 // s -> ns
        + data_.at(5)*8 // 8ns period -> ns
        + data_.at(6)/512 // 4096 ticks per 8ns period -> ns
        );
  }
  bool is_start_of_spill() const {
    if (data_.size() < 3) return false;
    return data_.at(2) == 1;
  }
  bool is_trigger() const {
    if (data_.size() < 3) return false;
    return data_.at(2) == 2;
  }
};

/**
 * White Rabbit raw data file
 *
 * ## Format
 * Each event has exactly seven (7) 32-bit words in it.
 * No extra signal headers or variable length, just 7 32-bit words.
 *
 * 0. Run number - run number according to white rabbit
 * 1. WR Counter - event counter local to White Rabbit
 * 2. Channel - channel/port being read from (table below)
 * 3. Seq ID - counter within a channel
 * 4. Seconds - whole IAT timestamp in seconds
 * 5. Coarse - coarse timestamp in 8 ns steps
 * 6. Frac - fractional timestamp in 8/4096 ns stemps
 *
 * ### channel descriptions
 *
 * Channel | Description
 * --------|------------
 * 1       | Start of spill
 * 2       | Scintillator plates (aka "telescope trigger")
 * 3       | High pressure cherenkov
 * 4       | Low pressure cherenkov
 * 5       | End of busy from ATCA
 *
 */
class WhiteRabbitRawFile : public reformat::RawDataFile {
  int i_spill{-1};
  unsigned long int last_spill_time_;
 public:
  WhiteRabbitRawFile(const framework::config::Parameters& ps);
  virtual std::optional<reformat::EventPacket> next() final override;
};  // WhiteRabbitRawFile

WhiteRabbitRawFile::WhiteRabbitRawFile(const framework::config::Parameters& ps)
  : RawDataFile(ps) {}

std::optional<reformat::EventPacket> WhiteRabbitRawFile::next() {
  WhiteRabbitEventPacket event;
  do {
    if (not (file_reader_ >> event)) {
      reformat_log(debug) << "no more events, ended with spill " << i_spill;
      return {};
    }
  } while (not event.is_start_of_spill() and not event.is_trigger());

  reformat::EventPacket ep;
  ep.append(event.data());

  if (event.is_start_of_spill()) { //and (event.timestamp() - last_spill_time_ > 5e9 or i_spill < 0)) {
    i_spill++;
    last_spill_time_ = event.timestamp();
    reformat_log(debug) << "new spill " << i_spill;
  }

  ep.setTimestamp(event.timestamp());
  
  return ep;
}

}

DECLARE_RAW_DATA_FILE(testbeam::WhiteRabbitRawFile);
