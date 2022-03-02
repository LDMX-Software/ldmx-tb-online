#ifndef EMULATORS_TRIGSCINT_SLOWCONTROLGENERATOR_H
#define EMULATORS_TRIGSCINT_SLOWCONTROLGENERATOR_H

#include "emulators/Generator.h"

namespace emulators {

// struct uint48_t {
//   uint64_t data : 48; 
// };

class TrigScint_SlowControlGenerator : public Generator {

public:
  static std::shared_ptr<TrigScint_SlowControlGenerator> create() {
    static std::shared_ptr<TrigScint_SlowControlGenerator> ret =
        std::make_shared<TrigScint_SlowControlGenerator>();
    return (ret);
  }

  /// Constructor
  TrigScint_SlowControlGenerator() : Generator(){};

  /// Default Constructor
  ~TrigScint_SlowControlGenerator() = default;

  /**
   * Generate a data frame of the give size.
   *
   * @param size The size of the frame in bytes.
   */
  void genFrame(uint32_t size);

  /** @return the TX count */
  uint32_t getTxCount() { return 0; }

  /** @return the total TX bytes */
  uint32_t getTxBytes() { return 0; }

  /** @return the total TX errors */
  uint32_t getTxErrors() { return 0; }

  void resetCounters() {}

private:
  /**
   */
  std::vector<uint16_t> buildQIESubpackets(int qie_count);

  /// Header lenght in bytes
  uint16_t header_size_{2};

  /// Trigger ID lenght
  uint16_t trigger_id_size_{2};

  /// Tail size
  uint16_t tail_size_{2};

  /// Number of QIE channels
  uint8_t qie_count_{0x1};

  /// QIE channel mask
  uint8_t qie_count_mask_{0xB};

  /// Format version
  uint8_t format_version_{0x1};

  /// Module ID
  uint8_t module_id_{0x1};

  /// Module ID mask
  uint8_t module_id_mask_{0x4};

  /// Card ID
  uint8_t card_id_{0x2};

  /// Card ID Mask
  uint8_t card_id_mask_{0x6};

  /// Event number
  uint16_t event_number_{0};

  //
  // QIE
  //

  /// ADC mask
  uint8_t adc_exp_mask_{0x6};
  
  /// ADC mask
  uint8_t tdc_mask_{0x8};
};

} // namespace emulators

#endif // EMULATORS_TRIGSCINT_SLOWCONTROLGENERATOR_H
