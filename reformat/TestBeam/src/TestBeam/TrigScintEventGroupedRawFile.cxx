
#include "Reformat/RawDataFile.h"

namespace testbeam {

/**
 * file that gets the timestamp and raw data from a file
 * that already has been grouped into events using the "decoding"
 * python script
 */
class TrigScintEventGroupedRawFile : public reformat::RawDataFile {
  int bytes_per_event_;
 public:
  TrigScintEventGroupedRawFile(const framework::config::Parameters& p);
  virtual std::optional<reformat::EventPacket> next() final override;
};

TrigScintEventGroupedRawFile::TrigScintEventGroupedRawFile(const framework::config::Parameters& p)
  : reformat::RawDataFile(p) {
    bytes_per_event_ = p.getParameter<int>("bytes_per_event");
  }

std::optional<reformat::EventPacket> TrigScintEventGroupedRawFile::next() {
  std::vector<uint8_t> buff;
  if (not file_reader_.read(buff, bytes_per_event_)) {
    reformat_log(debug) << "done with events";
    return {};
  }

  reformat::EventPacket ep;
  ep.append(buff);

  // first 8 bytes are the two deprecated timestamps (UTC seconds and UTC clock ticks)
  static const std::size_t TIMESINCESPILL_POS = 0;
  // next 4 bytes are the time since spill
  static const std::size_t TIMESINCESPILL_LEN_BYTES = 4;
  uint32_t timeSpill=0;
  for (int iW = 0; iW < TIMESINCESPILL_LEN_BYTES; iW++) {
    int pos = TIMESINCESPILL_POS + iW;
    timeSpill |= (buff.at(pos) << iW*8); //shift by a byte at a time
  }
  reformat_log(debug) << "time since spill " << timeSpill;
  ep.setTimestamp(timeSpill);
}

}

DECLARE_RAW_DATA_FILE(testbeam::TrigScintEventGroupedRawFile);
