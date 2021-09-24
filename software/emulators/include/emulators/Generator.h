#ifndef EMULATORS_GENERATOR_H
#define EMULATORS_GENERATOR_H

//---< rogue >---//
#include "rogue/interfaces/stream/Master.h"

namespace emulators {
class Generator : public rogue::interfaces::stream::Master {
public:
  /**
   */
  virtual void genFrame(uint32_t size) = 0;

  /**
   */
  virtual uint32_t getTxCount() = 0;

  /**
   */
  virtual uint32_t getTxBytes() = 0;

  /**
   */
  virtual uint32_t getTxErrors() = 0;
};
} // namespace emulators
#endif // EMULATORS_GENERATORS_H
