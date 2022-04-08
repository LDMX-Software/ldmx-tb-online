
//---< ldmx-eudaq >---//
#include "eudaq/RogueDataSender.h"
#include "boost/circular_buffer.hpp"

#include <mutex>

namespace eudaq {
class CaptanCircularBufferDataSender : public RogueDataSender {


 public:

  static std::shared_ptr<CaptanCircularBufferDataSender> create(eudaq::Producer *producer) {
    static std::shared_ptr<CaptanCircularBufferDataSender> ret =
        std::make_shared<CaptanCircularBufferDataSender>(producer);
    return (ret);
  }

  /// Constructor
  CaptanCircularBufferDataSender(eudaq::Producer *producer)
      : RogueDataSender(producer)
      , mCircularBuffer(1e8*sizeof(uint64_t))
  {};

  /// Default Constructor
  ~CaptanCircularBufferDataSender() = default;

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

  boost::circular_buffer<char> mCircularBuffer;
  std::unique_ptr<std::mutex> mBufferLock;
  
  size_t writeIntoCircularBuffer(const char* data_loc, size_t data_size);
  size_t readFromCircularBuffer();
  size_t deleteFromCircularBuffer(size_t data_size);
  
};
} // namespace eudaq
