#ifndef EUDAQ_TRIGSCINTTESTBEAMMONITOR_H
#define EUDAQ_TRIGSCINTTESTBEAMMONITOR_H

//---< eudaq >---//
#include "eudaq/ROOTMonitor.hh"

#include "TH2.h"

namespace eudaq {

class TrigScintTestBeamMonitor : public eudaq::ROOTMonitor {
public:
  TrigScintTestBeamMonitor(const std::string &name, const std::string &runcontrol)
    : eudaq::ROOTMonitor(name, "TrigScintTestBeamMonitor", runcontrol) {}

  ~TrigScintTestBeamMonitor() = default;

  void AtConfiguration() override; 
  void AtEventReception(eudaq::EventSP ev) override; 

  static const uint32_t factory_id_{eudaq::cstr2hash("TrigScintTestBeamMonitor")};
  
private:
  int getField(int value, int high_bit, int low_bit) {
    int mask{static_cast<int>(pow(2, (high_bit - low_bit + 1)) - 1)};
    return (value >> low_bit) & mask;
  }
  static const unsigned int comma_char=0xFBF7;
  std::vector<uint16_t> event_buffer1,event_buffer2;
  std::map<std::string, TH2D*> histo_map; 
};
} // namespace eudaq
#endif // EUDAQ_TRIGSCINTTESTBEAMMONITOR_H
