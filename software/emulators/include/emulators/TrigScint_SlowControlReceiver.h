#ifndef EMULATORS_TRIGSCINT_SLOWCONTROLRECEIVER_H
#define EMULATORS_TRIGSCINT_SLOWCONTROLRECEIVER_H

#include "emulators/Receiver.h"

namespace emulators {

class TrigScint_SlowControlReceiver : public Receiver {

public:
  static std::shared_ptr<TrigScint_SlowControlReceiver> create() {
    static std::shared_ptr<TrigScint_SlowControlReceiver> ret =
        std::make_shared<TrigScint_SlowControlReceiver>();
    return (ret);
  }

  /// Constructor
  TrigScint_SlowControlReceiver() : Receiver(){};

  /// Default Constructor
  ~TrigScint_SlowControlReceiver() = default;

  /**
   */
  void acceptFrame(std::shared_ptr<rogue::interfaces::stream::Frame> frame);
  
};
}

#endif // EMULATORS_TRIGSCINT_SLOWCONTROLRECEIVER_H
