#ifndef EUDAQ_SIMPLEMONITOR_H
#define EUDAQ_SIMPLEMONITOR_H

//---< eudaq >---//
#include "eudaq/ROOTMonitor.hh"

namespace eudaq {

class SimpleMonitor : public eudaq::ROOTMonitor {
public:
  SimpleMonitor(const std::string &name, const std::string &runcontrol)
    : eudaq::ROOTMonitor(name, "SimpleMonitor", runcontrol) {}

  ~SimpleMonitor() = default;

  void AtConfiguration() override {}; 
  void AtEventReception(eudaq::EventSP ev) override; 

  static const uint32_t factory_id_{eudaq::cstr2hash("SimpleMonitor")};
};
} // namespace eudaq
#endif // EUDAQ_SIMPLEMONITOR_H
