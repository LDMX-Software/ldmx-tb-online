#ifndef REFORMAT_EVENTPACKET_H
#define REFORMAT_EVENTPACKET_H

#include <vector>

namespace reformat {
/**
 * Each raw data file produced event packets
 */
class EventPacket {
 public:
  using TimestampType = uint64_t;
  static const TimestampType TimestampMax = 0xffffffffffffffff;
  const TimestampType& timestamp() const {
    return timestamp_;
  }
  void setTimestamp(const TimestampType& ts) {
    timestamp_ = ts;
  }
  std::vector<uint8_t>& data() {
    return data_;
  }
  template<typename WordType>
  void append(const WordType* a, std::size_t n) {
    std::size_t i{data_.size()};
    std::size_t l{n*sizeof(WordType)};
    data_.resize(i+l);
    std::memcpy(&(data_[i]), reinterpret_cast<const uint8_t*>(a), l);
  }
  template<typename WordType>
  void append(const WordType& w) {
    append(&w, 1);
  }
  template<typename WordType>
  void append(const std::vector<WordType>& vec) {
    append(vec.data(), vec.size());
  }
 private:
  /// the timestamp for this event **from this raw data file**
  TimestampType timestamp_;
  /// the buffer of raw data
  std::vector<uint8_t> data_;
}; // EventPacket
} // namespace reformat

#endif // REFORMAT_EVENTPACKET_H
