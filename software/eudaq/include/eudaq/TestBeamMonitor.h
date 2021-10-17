#ifndef EUDAQ_SIMPLEMONITOR_H
#define EUDAQ_SIMPLEMONITOR_H

//---< eudaq >---//
#include "eudaq/ROOTMonitor.hh"

namespace eudaq {

class TestBeamMonitor : public eudaq::ROOTMonitor {
public:
  TestBeamMonitor(const std::string &name, const std::string &runcontrol)
    : eudaq::ROOTMonitor(name, "TestBeamMonitor", runcontrol) {}

  ~TestBeamMonitor() = default;

  void AtConfiguration() override; 
  void AtEventReception(eudaq::EventSP ev) override; 

  static const uint32_t factory_id_{eudaq::cstr2hash("TestBeamMonitor")};
  
private:
  int getField(int value, int high_bit, int low_bit) {
    int mask{static_cast<int>(pow(2, (high_bit - low_bit + 1)) - 1)};
    return (value >> low_bit) & mask;
  }

  std::map<std::string, TH1D*> histo_map; 
};
} // namespace eudaq
#endif // EUDAQ_SIMPLEMONITOR_H
