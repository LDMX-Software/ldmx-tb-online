#ifndef EMULATORS_TRIGSCINTRECEIVER_H
#define EMULATORS_TRIGSCINTRECEIVER_H

#include "emulators/Receiver.h"

namespace emulators {

class TrigScintReceiver : public Receiver {

public:
  static std::shared_ptr<TrigScintReceiver> create() {
    static std::shared_ptr<TrigScintReceiver> ret =
        std::make_shared<TrigScintReceiver>();
    return (ret);
  }

  /// Constructor
  TrigScintReceiver() : Receiver(){};

  /// Default Constructor
  ~TrigScintReceiver() = default;

  /**
   */
  void acceptFrame(std::shared_ptr<rogue::interfaces::stream::Frame> frame);
  
};
}

#endif // EMULATORS_TRIGSCINTRECEIVER_H
