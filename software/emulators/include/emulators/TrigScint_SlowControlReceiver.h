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

  /**
   * @note Print QIE Information from 72-bit data
   */
  void PrintQIEInfo(std::vector<uint8_t> word_);

private:
  float TDC_Threshold_LSB = 0.156;   // least possibble TDC threshold [uA]. Might also be 3.74
  float LVDS_V_OUT[4] = {150, 250, 350, 450};
  float Gsel[13] = {3.1,4.65,6.2,9.3,12.4,15.5,18.6,21.7,24.8,27.9,31,34.1,35.65};

};
}

#endif // EMULATORS_TRIGSCINT_SLOWCONTROLRECEIVER_H
