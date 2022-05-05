#ifndef REFORMAT_EVENTPACKET_H
#define REFORMAT_EVENTPACKET_H

#include <vector>

namespace reformat {
/**
 * Each raw data file produced event packets
 */
class EventPacket {
  /// the timestamp for this event **from this raw data file**
  uint32_t timestamp_;
  /// the buffer of raw data
  std::vector<uint8_t> data_;
 public:
  const uint32_t& timestamp() const {
    return timestamp_;
  }
  void setTimestamp(const uint32_t& ts) {
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
}; // EventPacket
} // namespace reformat

#endif // REFORMAT_EVENTPACKET_H
