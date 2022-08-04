
#include "Reformat/RawDataFile.h"

namespace testbeam {

/**
 * Each "field" of data in a FiberTracker packet
 */
class FiberTrackerField {
  uint32_t field_header_;
  std::vector<uint32_t> field_value_;
 public:
  /**
   * r - reader
   * i_field - field we are supposed to be reading from
   */
  FiberTrackerField(reformat::utility::Reader& r, int i_field) {
    uint32_t len;
    r >> len >> field_header_;
    r.read(field_value_,len-1);
    if (i_field != field_header_) {
      EXCEPTION_RAISE("BadForm", "Field "+std::to_string(i_field)
          +" has a mismatched header "+std::to_string(field_header_));
    }
  }

  /**
   * conversion to a single int
   */
  int to_int(const std::size_t i = 0) const {
    return field_value_.at(i);
  }

  /**
   * conversion from series of ints to string specific to FiberTrackerDAQ
   */
  std::string to_string() const {
    std::string str;
    str.resize(field_value_.size());
    for (int i{0}; i < str.size(); i++) {
      str[i] = (char)field_value_[i];
    }
    return str;
  }
  
  /**
   * long split across two ints
   *
   * i is index of field value to start from
   */
  long int to_long(const std::size_t i = 0) const {
    return ((uint64_t)field_value_.at(i+1) << 32) | (uint64_t)field_value_.at(i);
  }

  /**
   * Get the field value
   */
  const std::vector<uint32_t>& value() const {
    return field_value_;
  }
}; // A field in the FiberTracker DAQ format

/**
 * A event from a FiberTracker station
 *
 * Each event is 10 32-bit words where
 *
 * Word 0: Timestamp LSB in 8 ns ticks since the last whole second, for the trigger signal
 * Word 1: Timestamp MSB in UNIX epoch seconds for the trigger signal (in international atomic time)
 *
 * Word 2: Timestamp LSB in 8 ns ticks since the last whole second, for the "event" 
 * Word 3: Timestamp MSB in UNIX epoch seconds for the "event"
 *  (honestly don't know what they mean by "event", so I ignore this timestamp)
 *
 * Word 4-9: Hit information, the fiber tracker has 6*32=192 fibers, a 1 means a hit and a 0 means no hit. 
 *
 * There are 192 bits to make the system compatible with the samll and big detectors.
 * In T9 we have small detectors (only 96 channels) so we have lots of zeros.
 * Only channels 48-144 have detector data, the other channels are empty (all zeros).
 */
struct FiberTrackerEvent {
  uint32_t trigger_timestamp_lsb,
           trigger_timestamp_msb,
           event_timestamp_lsb,
           event_timestamp_msb;
  std::vector<uint32_t> channel_hits;

  FiberTrackerEvent(const std::vector<uint32_t>& spill_data, std::size_t i_word) {
    trigger_timestamp_lsb = spill_data.at(i_word);
    trigger_timestamp_msb = spill_data.at(i_word+1);
    event_timestamp_lsb   = spill_data.at(i_word+2);
    event_timestamp_msb   = spill_data.at(i_word+3);
    channel_hits.clear();
    channel_hits.reserve(6);
    for (std::size_t i{i_word+4}; i < i_word+10 and i < spill_data.size(); i++)
      channel_hits.push_back(spill_data.at(i));
  }

  FiberTrackerEvent() {
    trigger_timestamp_lsb = 0xffffffff;
    trigger_timestamp_msb = 0xffffffff;
    event_timestamp_lsb   = 0xffffffff;
    event_timestamp_msb   = 0xffffffff;
    channel_hits.clear();
    channel_hits.reserve(6);
    for (int i=0; i < 6; i++)
      channel_hits.push_back(0xffffffff);
  }
};

/**
 * Fiber Tracker raw data file
 *
 * ## Format
 * The fiber tracker records data on a spill-by-spill basis.
 * Each spill has 25 "fields" in the same order every time.
 *
 * The list of event-by-event data is the 14'th field in a spill.
 */
class FiberTrackerRawFile : public reformat::RawDataFile {
 public:
  FiberTrackerRawFile(const framework::config::Parameters& ps);
  virtual std::optional<reformat::EventPacket> next() final override;
 private:
  void next_spill();
 private:
  bool suddenStop = false;
  int i_spill_event_;
  std::vector<FiberTrackerEvent> spill_events_;
};  // FiberTrackerRawFile

FiberTrackerRawFile::FiberTrackerRawFile(const framework::config::Parameters& ps)
  : RawDataFile(ps) {}

std::optional<reformat::EventPacket> FiberTrackerRawFile::next() {
  i_spill_event_++;
  if (i_spill_event_ >= spill_events_.size()) {
    

    //TODO Temporary hack to recover from faulty end of file
    if (file_reader_ && !suddenStop) {
      reformat_log(fatal) << "Reading new spill\n";
      next_spill();
      i_spill_event_ = 0;
    } else {
      return {};
    }
  }
  const FiberTrackerEvent& ft_event{spill_events_.at(i_spill_event_)};

  reformat::EventPacket ep;
  ep.append(ft_event.trigger_timestamp_lsb);
  ep.append(ft_event.trigger_timestamp_msb);
  ep.append(ft_event.event_timestamp_lsb);
  ep.append(ft_event.event_timestamp_msb);
  ep.append(ft_event.channel_hits);

  // calculate FiberTracker TimeStamp
  reformat::EventPacket::TimestampType ts{ft_event.trigger_timestamp_msb};
  ts <<= 32;
  ts |= ft_event.trigger_timestamp_lsb;
  ep.setTimestamp(ts);
  
  return ep;
}

void FiberTrackerRawFile::next_spill() {
  int i_field{0};
  try{
    int acqMode = FiberTrackerField(file_reader_,++i_field).to_int();
    reformat_log(debug) << i_field << " acqMode = " << acqMode;
  }
  catch(framework::exception::Exception e){
    suddenStop = true;
    return;
  }
  long int acqStamp = FiberTrackerField(file_reader_,++i_field).to_long();
  reformat_log(debug) << i_field << " acqStamp = " << acqStamp;
  int acqType = FiberTrackerField(file_reader_,++i_field).to_int();
  reformat_log(debug) << i_field << " acqType = " << acqType;
  int acqTypeAllowed = FiberTrackerField(file_reader_,++i_field).to_int();
  reformat_log(debug) << i_field << " acqTypeAllowed = " << acqTypeAllowed;
  std::string coincidenceInUse = FiberTrackerField(file_reader_,++i_field).to_string();
  reformat_log(debug) << i_field << " coincidenceInUse = " << coincidenceInUse;
  int counts = FiberTrackerField(file_reader_,++i_field).to_int();
  reformat_log(debug) << i_field << " counts = " << counts;
  long int countsRecords = FiberTrackerField(file_reader_,++i_field).to_long();
  reformat_log(debug) << i_field << " countsRecords = " << countsRecords;
  long int countsRecordsWithZeroEvents = FiberTrackerField(file_reader_,++i_field).to_long();
  reformat_log(debug) << i_field << " countsRecordsWithZeroEvents = " << countsRecordsWithZeroEvents;
  long int countsTrigs = FiberTrackerField(file_reader_,++i_field).to_long();
  reformat_log(debug) << i_field << " countsTrigs = " << countsTrigs;
  std::string cycleName = FiberTrackerField(file_reader_,++i_field).to_string();
  reformat_log(debug) << i_field << " cycleName = " << cycleName;
  long int cycleStamp = FiberTrackerField(file_reader_,++i_field).to_long();
  reformat_log(debug) << i_field << " cycleStamp = " << cycleStamp;
  std::string equipmentName = FiberTrackerField(file_reader_,++i_field).to_string();
  reformat_log(debug) << i_field << " equipmentName = " << equipmentName;
  int eventSelectionAcq = FiberTrackerField(file_reader_,++i_field).to_int();
  reformat_log(debug) << i_field << " eventSelectionAcq = " << eventSelectionAcq;
  FiberTrackerField events_data_field(file_reader_,++i_field);
  spill_events_.clear();
  //spill_events_.reserve( (events_data_field.value().size())/10);
  reformat_log(debug) << i_field << " spill_data_field.value().size(): " << events_data_field.value().size() << ")";
  for (std::size_t i_word{0}; i_word < events_data_field.value().size(); i_word += 10) {
    spill_events_.emplace_back(events_data_field.value(), i_word);
  }
  //Add spill delimeter to mark the end of a spill, which is exactly the default constructor of the FiberTrackerEvent
  spill_events_.emplace_back();

  reformat_log(debug) << i_field << " spill_events_ (size = " << spill_events_.size() << ")";
  // should be double technically
  FiberTrackerField meanSNew_field(file_reader_,++i_field);
  reformat_log(debug) << i_field << " meanSNew";
  std::string message = FiberTrackerField(file_reader_,++i_field).to_string();
  reformat_log(debug) << i_field << " message = " << message;
  // array of doubles, not translated
  FiberTrackerField profile_field(file_reader_,++i_field);
  reformat_log(debug) << i_field << " profile";
  // fields 18 and 19 are skipped
  i_field += 2;
  // array of doubles, not translated
  FiberTrackerField profileStandAlone_field(file_reader_,++i_field);
  reformat_log(debug) << i_field << " profileStandAlone";
  std::string timeFirstEvent = FiberTrackerField(file_reader_,++i_field).to_string();
  reformat_log(debug) << i_field << " timeFirstEvent = " << timeFirstEvent;
  std::string timeFirstTrigger = FiberTrackerField(file_reader_,++i_field).to_string();
  reformat_log(debug) << i_field << " timeFirstTrigger = " << timeFirstTrigger;
  std::string timeLastEvent = FiberTrackerField(file_reader_,++i_field).to_string();
  reformat_log(debug) << i_field << " timeLastEvent = " << timeLastEvent;
  std::string timeLastTrigger = FiberTrackerField(file_reader_,++i_field).to_string();
  reformat_log(debug) << i_field << " timeLastTrigger = " << timeLastTrigger;
  int trigger = FiberTrackerField(file_reader_,++i_field).to_int();
  reformat_log(debug) << i_field << " trigger = " << trigger;
  int triggerOffsetAcq = FiberTrackerField(file_reader_,++i_field).to_int();
  reformat_log(debug) << i_field << " triggerOffsetAcq = " << triggerOffsetAcq;
  int triggerSelectionAcq = FiberTrackerField(file_reader_,++i_field).to_int();
  reformat_log(debug) << i_field << " triggerSelectionAcq = " << triggerSelectionAcq;
}

}

DECLARE_RAW_DATA_FILE(testbeam::FiberTrackerRawFile);
