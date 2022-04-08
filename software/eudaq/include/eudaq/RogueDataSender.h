#ifndef EUDAQ_ROGUEDATASENDER_H
#define EUDAQ_ROGUEDATASENDER_H

//---< StdLib >---//
#include <string>
#include <cmath>

//---< eudaq >---//
#include "eudaq/Producer.hh"

//---< rogue >---//
#include "rogue/interfaces/stream/Slave.h"

namespace eudaq {
class RogueDataSender : public rogue::interfaces::stream::Slave {
public:
  /// Constructor
  RogueDataSender(eudaq::Producer *producer) { producer_ = producer; }

  /// Default Constructor
  ~RogueDataSender() = default;

  /** @return the RX count */
  virtual uint32_t getRxCount() const { return rx_count_; };

  /** @return the total RX bytes */
  virtual uint32_t getRxBytes() const { return rx_bytes_; };

  /** @return the total RX errors */
  virtual uint32_t getRxErrors() const { return rx_error_count_; };

  /**
   */
  void acceptFrame(std::shared_ptr<rogue::interfaces::stream::Frame> frame);

  /**
   * Process the data in the frame and send it to the associated data collector.
   *
   * @param frame The rogue frame containing the data
   */
  virtual void sendEvent(std::shared_ptr<rogue::interfaces::stream::Frame> frame) = 0;

protected:
  /** 
   * Extract a field that lies within the range high_bit to low_bit of a 
   * 32 bit int.
   *
   * @param vlaue The int to extract a field from.
   * @param high_bit The highest bit index of the field [0-31].
   * @param low_bit The lower bit index of the field [0-31].
   */
  int getField(int value, int high_bit, int low_bit) {
    int mask{static_cast<int>(pow(2, (high_bit - low_bit + 1)) - 1)};
    return (value >> low_bit) & mask;
  }

  /// The producer to use to send data
  eudaq::Producer *producer_{nullptr};

  /// RX count
  uint32_t rx_count_{0};

  /// RX bytes
  uint32_t rx_bytes_{0};

  /// RX errors
  uint32_t rx_error_count_{0};

};
} // namespace eudaq
#endif // EUDAQ_ROGUEDATASENDER_H
