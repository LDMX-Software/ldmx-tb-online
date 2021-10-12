#ifndef EUDAQ_ROGUEDATASENDER_H
#define EUDAQ_ROGUEDATASENDER_H

//---< StdLib >---//
#include <string>

//---< eudaq >---//
#include "eudaq/Producer.hh"

//---< rogue >---//
#include "rogue/interfaces/stream/Slave.h"

namespace eudaq {
class RogueDataSender : public rogue::interfaces::stream::Slave {
public:
  static std::shared_ptr<RogueDataSender> create(eudaq::Producer *producer) {
    static std::shared_ptr<RogueDataSender> ret =
        std::make_shared<RogueDataSender>(producer);
    return (ret);
  }

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

private:
  /// RX count
  uint32_t rx_count_{0};

  /// RX bytes
  uint32_t rx_bytes_{0};

  /// RX errors
  uint32_t rx_error_count_{0};

  /// The producer to use to send data
  eudaq::Producer *producer_{nullptr};
};
} // namespace eudaq
#endif // EUDAQ_ROGUEDATASENDER_H
