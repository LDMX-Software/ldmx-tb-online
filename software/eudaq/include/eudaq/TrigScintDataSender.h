
#include <vector>

#include "eudaq/RogueDataSender.h"

namespace eudaq {
class TrigScintDataSender : public RogueDataSender {
public:
  static std::shared_ptr<TrigScintDataSender> create(eudaq::Producer *producer) {
    static std::shared_ptr<TrigScintDataSender> ret =
        std::make_shared<TrigScintDataSender>(producer);
    return (ret);
  }

  /// Constructor
  TrigScintDataSender(eudaq::Producer *producer) : RogueDataSender(producer){};

  /// Default Constructor
  ~TrigScintDataSender() = default;

  /**
   * Process the data in the frame and send it to the associated data collector.
   * In the case of the TrigScint, it simply extracts the readout request and sets
   * that to the trigger number. The timestamp is also set.
   *
   * @param frame The rogue frame containing the data
   */
  virtual void
  sendEvent(std::shared_ptr<rogue::interfaces::stream::Frame> frame);

 private: 
  /// Buffer to hold trigger scintillator data. This is needed because events
  /// are split across packets.
  std::vector<uint64_t> buffer; 
};
} // namespace eudaq
