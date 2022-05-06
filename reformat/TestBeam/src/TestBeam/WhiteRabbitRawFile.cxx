
#include "Reformat/RawDataFile.h"

namespace testbeam {

/**
 * White Rabbit raw data file
 *
 * ## Format
 * Each event has exactly seven (7) 32-bit words in it.
 * No extra signal headers or variable length, just 7 32-bit words.
 *
 * 1. Run number - run number according to white rabbit
 * 2. WR Counter - event counter local to White Rabbit
 * 3. Channel ???
 * 4. Seq ID ???
 * 5. Seconds ???
 * 6. Coarse ???
 * 7. Frac ???
 */
class WhiteRabbitRawFile : public reformat::RawDataFile {
 public:
  WhiteRabbitRawFile(const framework::config::Parameters& ps);
  virtual std::optional<reformat::EventPacket> next() final override;
};  // WhiteRabbitRawFile

WhiteRabbitRawFile::WhiteRabbitRawFile(const framework::config::Parameters& ps)
  : RawDataFile(ps) {}

std::optional<reformat::EventPacket> WhiteRabbitRawFile::next() {
  if (!file_reader_ or file_reader_.eof()) return {};

  std::vector<uint32_t> event_data;
  if (!file_reader_.read(event_data, 7)) {
    reformat_log(error) << "File ended in the middle of reading an event.";
    return {};
  }

  reformat::EventPacket ep;
  ep.append(event_data);

  // calculate WR timestamp
  reformat::EventPacket::TimestampType ts{event_data.at(4)};
  ep.setTimestamp(ts);
  
  return ep;
}

}

DECLARE_RAW_DATA_FILE(testbeam::WhiteRabbitRawFile);
