
//---< ldmx-eudaq >---//
#include "eudaq/RogueDataSender.h"

namespace eudaq {
class HCalDataSender : public RogueDataSender {
public:
  static std::shared_ptr<HCalDataSender> create(eudaq::Producer *producer) {
    static std::shared_ptr<HCalDataSender> ret =
        std::make_shared<HCalDataSender>(producer);
    return (ret);
  }

  /// Constructor
  HCalDataSender(eudaq::Producer *producer) : RogueDataSender(producer){};

  /// Default Constructor
  ~HCalDataSender() = default;

  /**
   * Process the data in the frame and send it to the associated data collector.
   * In the case of the HCal, it simply extracts the readout request and sets
   * that to the trigger number. The timestamp is also set.
   *
   * @param frame The rogue frame containing the data
   */
  virtual void
  sendEvent(std::shared_ptr<rogue::interfaces::stream::Frame> frame);
};
} // namespace eudaq
