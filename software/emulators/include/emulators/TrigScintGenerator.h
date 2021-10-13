#ifndef EMULATORS_TRIGSCINTGENERATOR_H
#define EMULATORS_TRIGSCINTGENERATOR_H

#include "emulators/Generator.h"

namespace emulators {

struct uint48_t {
  uint64_t data : 48; 
};

class TrigScintGenerator : public Generator {

public:
  static std::shared_ptr<TrigScintGenerator> create() {
    static std::shared_ptr<TrigScintGenerator> ret =
        std::make_shared<TrigScintGenerator>();
    return (ret);
  }

  /// Constructor
  TrigScintGenerator() : Generator(){};

  /// Default Constructor
  ~TrigScintGenerator() = default;

  /**
   * Generate a data frame of the give size.
   *
   * @param size The size of the frame in bytes.
   */
  void genFrame(uint32_t size);

  /** @return the TX count */
  uint32_t getTxCount(){return 0;};

  /** @return the total TX bytes */
  uint32_t getTxBytes(){return 0;};

  /** @return the total TX errors */
  uint32_t getTxErrors(){return 0;};

private:
  /// Event number
  uint16_t event_number_{0};
};

} // namespace emulators

#endif // EMULATORS_TRIGSCINTGENERATOR_H
