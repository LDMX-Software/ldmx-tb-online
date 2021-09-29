#ifndef EMULATORS_RECEIVER_H
#define EMULATORS_RECEIVER_H

//---< rogue >---//
#include "rogue/interfaces/stream/Slave.h"

namespace emulators {
class Receiver : public rogue::interfaces::stream::Slave {
public:
  /**
   */
  virtual void
  acceptFrame(std::shared_ptr<rogue::interfaces::stream::Frame> frame) = 0;

  /** @return the RX count */
  virtual uint32_t getRxCount() const { return rx_count_; };

  /** @return the total RX bytes */
  virtual uint32_t getRxBytes() const { return rx_bytes_; };

  /** @return the total RX errors */
  virtual uint32_t getRxErrors() const { return rx_error_count_; };

protected:
  int getField(int value, int high_bit, int low_bit) {
    int mask{static_cast<int>(pow(2, (high_bit - low_bit + 1)) - 1)};
    return (value >> low_bit) & mask;
  }

  /// RX count
  uint32_t rx_count_{0};

  /// RX bytes
  uint32_t rx_bytes_{0};

  /// RX errors
  uint32_t rx_error_count_{0};
};
} // namespace emulators
#endif // EMULATORS_RECEIVERS_H
