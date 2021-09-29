#ifndef EMULATORS_HCALRECEIVER_H
#define EMULATORS_HCALRECEIVER_H

#include "emulators/Receiver.h"

namespace emulators {

class HCalReceiver : public Receiver {

public:
  static std::shared_ptr<HCalReceiver> create() {
    static std::shared_ptr<HCalReceiver> ret =
        std::make_shared<HCalReceiver>();
    return (ret);
  }

  /// Constructor
  HCalReceiver() : Receiver(){};

  /// Default Constructor
  ~HCalReceiver() = default;

  /**
   */
  void acceptFrame(std::shared_ptr<rogue::interfaces::stream::Frame> frame);
  
};
}

#endif // EMULATORS_HCALRECEIVER_H
