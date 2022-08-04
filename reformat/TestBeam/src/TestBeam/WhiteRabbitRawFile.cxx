
#include "Reformat/RawDataFile.h"

namespace testbeam {

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
  int events_in_spill = 0;
  unsigned long int last_spill_time_;
 public:
  WhiteRabbitRawFile(const framework::config::Parameters& ps);
  virtual std::optional<reformat::EventPacket> next() final override;
};  // WhiteRabbitRawFile

WhiteRabbitRawFile::WhiteRabbitRawFile(const framework::config::Parameters& ps)
  : RawDataFile(ps) {}

std::optional<reformat::EventPacket> WhiteRabbitRawFile::next() {
  std::vector<uint32_t> event_data;

  if (!file_reader_.read(event_data, 7)) {
    reformat_log(debug) << "no more events, ended with spill " << i_spill;
    return {};
  }
  
  reformat::EventPacket ep;
  //TODO ep.append(event_data) did not work, but wondering if this can be done more elegantly
  for(int i = 0; i < 7; i++){
    ep.append(event_data.at(i));
  }

  if (event_data.at(2) == 1 and (event_data.at(4) - last_spill_time_ > 5 or i_spill < 0)) {
    i_spill++;
    last_spill_time_ = event_data.at(4)*1e9 + event_data.at(5)*8;
    reformat_log(debug) << "new spill " << i_spill << " with " << events_in_spill << "events\n";
    events_in_spill = 0;
  }

  // calculate WR timestamp
  reformat::EventPacket::TimestampType ts{event_data.at(4)};
  ts = ts << 32;
  ts |= event_data.at(5);
  ep.setTimestamp(ts);
  
  return ep;
}

}

DECLARE_RAW_DATA_FILE(testbeam::WhiteRabbitRawFile);
