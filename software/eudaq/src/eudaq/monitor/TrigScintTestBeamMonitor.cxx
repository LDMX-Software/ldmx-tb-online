#include "TrigScintTestBeamMonitor.h"

#include <iostream>

namespace {
auto dummy0 = eudaq::Factory<eudaq::Monitor>::Register<
    eudaq::TrigScintTestBeamMonitor, const std::string &, const std::string &>(
    eudaq::TrigScintTestBeamMonitor::factory_id_);
}

namespace eudaq {

void TrigScintTestBeamMonitor::AtConfiguration() {
  /**
   * Book plots here.
   */
}

void TrigScintTestBeamMonitor::AtEventReception(EventSP event) {
  /**
   * Drop code to extract samples here.
   */
}
} // namespace eudaq
