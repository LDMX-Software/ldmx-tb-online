
#include "Reformat/RawDataFile.h"
#include "Reformat/Utility/Mask.h"
#include "Reformat/Utility/Hex.h"

namespace testbeam {

/**
 * Smallest "chunk" of data coming from TS frontend
 *
 * Represents a single sample of data collected from the
 * scintillator bars.
 *
 * Similar to qie_frame from legacy python script
 */
struct TimeSample {
  std::vector<uint8_t> adcs;
  std::vector<uint8_t> tdcs;
  int capid, ce, bc0;

  /**
   * Parse the input stream of bytes into this time sample
   *
   * @note The input buffer needs to be _exactly_ 11 bytes.
   *
   * ## Format
   * Each row is a BYTE (two hex digits aka two nibbles, 8 bits)
   *
   *  | ???? | capid (2 bits) | ce (1 bit) | bc0 (1 bit) |
   *  | adc0 |
   *  | adc1 |
   *  | .... |
   *  | adc8 |
   *  | letdc0 | letdc1 | letdc2 | letdc3 |
   *  | letdc4 | letdc5 | letdc6 | letdc7 |
   *
   * this is LETDC; only the two most significant bits included
   *    they are shipped as least significant bits --> shift them 
   *    want LE TDC = 3 to correspond to 64 > 49 (which is maxTDC in sim)
   */
  TimeSample(const std::vector<uint8_t>& buffer) {
    capid = (buffer.at(0) >> 2) & 0x3;
    ce    = (buffer.at(0) >> 1) & 0x1;
    bc0   = (buffer.at(0) >> 0) & 0x1;
    for (std::size_t i_channel{0}; i_channel < 8; i_channel++) {
      adcs.push_back(buffer.at(1+i_channel));
      uint8_t tdc_pack = buffer.at(9 + 1*(i_channel>3));
      uint8_t letdc = ((tdc_pack >> (2*(i_channel%4))) & 0xff);
      tdcs.push_back(16*(letdc+1));
    }
  } // decoding constructor

};  // TimeSample

/**
 * file that gets the timestamp and raw data from a file
 * that was written directly from eudaq, i.e. the data is still unsorted
 */
class TrigScintTwoFibersRawFile : public reformat::RawDataFile {
  int i_ts_event_{0};
  std::vector<TimeSample> extract_timesamples(const std::vector<uint32_t>& stream);
 public:
  TrigScintTwoFibersRawFile(const framework::config::Parameters& p);
  virtual std::optional<reformat::EventPacket> next() final override;
};

/**
 * clean and split the raw fiber data into time samples
 *
 * The fiber data is "contaminated" with different signal phrases.
 * - 0xfbf7 : needs to be removed
 * - 0x7cbc : needs to be removed
 * - 0xbc   : separates time samples within a fiber
 *
 * After this cleaning and splitting, we provide the encoded time sample
 * bytes to the TimeSample constructor to decode into data.
 */
std::vector<TimeSample> TrigScintTwoFibersRawFile::extract_timesamples(
    const std::vector<uint32_t>& stream) {
  // clean
  std::vector<uint16_t> cleaned;
  for (const uint32_t& word : stream) {
    for (std::size_t i{0}; i < 2; i++) {
      uint16_t subword = ((word >> 16*i) & 0xffff);
      if (subword != 0xf7fb and subword != 0xbc7c) 
        cleaned.push_back(subword);
    }
  }

  // split
  std::vector<std::vector<uint8_t>> timesamples;
  timesamples.emplace_back(); // need first one
  for (const uint16_t& word : cleaned) {
    uint8_t b0 = ((word >> 8) & 0xff);
    uint8_t b1 = ((word >> 0) & 0xff);
    for (uint8_t byte : {b0,b1}) {
      if (byte == 0xbc) {
        // new timesample
        reformat_log(debug) << "New Timesample, previous one has " 
          << timesamples.back().size() << " bytes";
        timesamples.emplace_back();
      } else {
        timesamples.back().push_back(byte);
      }
    }
  }
  reformat_log(debug) << "Last time sample has " << timesamples.back().size() << " bytes";

  // decode
  std::vector<TimeSample> decoded_ts;
  for (const auto& raw_ts : timesamples) {
    if (raw_ts.size() == 11) {
      decoded_ts.emplace_back(raw_ts);
    } else {
      reformat_log(debug) << "Received " << raw_ts.size() << " != 11 bytes for a time sample";
    }
  }

  return decoded_ts;
}

TrigScintTwoFibersRawFile::TrigScintTwoFibersRawFile(const framework::config::Parameters& p)
  : reformat::RawDataFile(p) {
    reformat_log(debug) << "throwing away words until first event signal word";
    uint64_t w{0};
    while (file_reader_ and w != 0xffffffffffffffff) file_reader_ >> w;
  }

std::optional<reformat::EventPacket> TrigScintTwoFibersRawFile::next() {
  /**
   * the first 64-bit word has the timestamp burried within it
   *
   * if we can't extract 8 more bytes, we assume the file is done and return
   * the ending condition.
   *
   * The endian-ness is flopped so we need to do this nonsense with a byte array
   */
  uint8_t bytes[8];
  for (std::size_t i{8}; i > 0; i--) file_reader_ >> bytes[i-1];
  if (not file_reader_) {
    reformat_log(debug) << "done with events";
    return {};
  }

  reformat::EventPacket ep;
  uint64_t zero{0};
  ep.append(zero); // UTC seconds
  ep.append(zero); // ns clock ticks since last whole second
  ep.append(bytes, 8); // time since spill

  reformat::EventPacket::TimestampType ts;
  std::memcpy(bytes, &ts, 8);
  reformat_log(debug) << "time since spill " << ts << " " << reformat::utility::hex(ts);
  ep.setTimestamp(ts);

  /**
   * We read 64-bit words until the boundary word (all F's) is encountered
   *
   * We assume the last boundary word was read by the last event.
   * @note This means the first event needs to handle initializing the reader.
   *
   * ## Format Notes
   * Each row is a 64-bit word.
   *
   *  |    ?nonsense?     | Time Since Spill (32 bits) |
   *  | Fiber 1 (32 bits) | Fiber 2 (32 bits) |
   *  |  ...continue...   |  ...continue...   |
   *  | End of Event Signal Word (All 1s)     |
   *
   * Look at TimeSample for how to decode the
   * two "columns" of raw fiber data.
   * The fiber data needs to be byte flopped like the time
   * stamp word above.
   */
  std::vector<uint32_t> fiber_1_raw, fiber_2_raw;
  uint64_t w{0};
  while (file_reader_ and w != 0xffffffffffffffff) {
    file_reader_ >> w;
    std::memcpy(&w, bytes, 8);
    fiber_1_raw.push_back((bytes[4]<<24)+(bytes[5]<<16)+(bytes[6]<<8)+bytes[7]);
    fiber_2_raw.push_back((bytes[0]<<24)+(bytes[1]<<16)+(bytes[2]<<8)+bytes[3]);
  }

  reformat_log(debug) << "done reading raw fiber data "
    << fiber_1_raw.size() << " fiber 1 words and "
    << fiber_2_raw.size() << " fiber 2 words";

  std::vector<TimeSample> fiber_1{extract_timesamples(fiber_1_raw)},
                          fiber_2{extract_timesamples(fiber_2_raw)};

  if (fiber_1.size() != fiber_2.size()) {
    reformat_log(error) << "Non-matching number of time samples between fibers!";
    return {};
  }

  reformat_log(debug) << fiber_1.size() << " time samples in this event";

  /// error codes, calculated during decoding
  bool crc1_error{false}, crc2_error{false}, cid_unsync{false}, cid_skip{false};
  /**
   * adc and tdc readout map in event
   *
   * Resort the samples into by-channel rather than
   * by time sample. While sorting, we also check for
   * various error codes.
   *
   * key: int electronics ID of channel
   * val: vector of adc/tdc indexed by time sample
   */
  std::map<int, std::vector<int>> adc_map, tdc_map;
  for (std::size_t i_ts{0}; i_ts < fiber_1.size(); i_ts++) {
    if (fiber_1.at(i_ts).capid != fiber_2.at(i_ts).capid) cid_unsync = true;
    if (fiber_1.at(i_ts).ce != 0) crc1_error = true;
    if (fiber_2.at(i_ts).ce != 0) crc2_error = true;
    if (i_ts > 0) {
      // logic needs some work, what happens if corrupt time sample word in middle?
      if ((fiber_1.at(i_ts-1).capid+1)%4 != fiber_1.at(i_ts).capid%4) {
        reformat_log(debug) << "Found CIDSkip in fiber 1";
        cid_skip = true;
      }
      if ((fiber_2.at(i_ts-1).capid+1)%4 != fiber_2.at(i_ts).capid%4) {
        reformat_log(debug) << "Found CIDSkip in fiber 2";
        cid_skip = true;
      }
    } // check if corrupted word
    for (std::size_t i_c{0}; i_c < 8; i_c++) {
      adc_map[i_c  ].push_back(fiber_1.at(i_ts).adcs.at(i_c));
      adc_map[i_c+8].push_back(fiber_2.at(i_ts).adcs.at(i_c));
      tdc_map[i_c  ].push_back(fiber_1.at(i_ts).tdcs.at(i_c));
      tdc_map[i_c+8].push_back(fiber_2.at(i_ts).tdcs.at(i_c));
    } // loop over channels in a fiber
  } // loop over time samples

  // three bytes for event number
  uint8_t event_number_bytes[3];
  std::memcpy(&i_ts_event_, event_number_bytes, 3);
  ep.append(event_number_bytes, 3);
  i_ts_event_++;

  // one byte with error codes
  uint8_t error_byte = 0;
  error_byte |= (crc1_error << 0);
  error_byte |= (crc2_error << 1);
  error_byte |= (cid_unsync << 2);
  error_byte |= (cid_skip   << 3);
  ep.append(error_byte);

  uint8_t data;
  for (const auto& [i, adcs] : adc_map) {
    if (adcs.size() != 16) {
      reformat_log(debug) << "got " << adcs.size() << " != 16 words in event data";
    }
    for (int adc : adcs) {
      std::memcpy(&adc,&data,1);
      ep.append(data);
    }
  }

  for (const auto& [i, tdcs] : tdc_map) {
    if (tdcs.size() != 16) {
      reformat_log(debug) << "got " << tdcs.size() << " != 16 words in event data";
    }
    for (int tdc : tdcs) {
      std::memcpy(&tdc,&data,1);
      ep.append(data);
    }
  }

  return ep;
}

}

DECLARE_RAW_DATA_FILE(testbeam::TrigScintTwoFibersRawFile);
