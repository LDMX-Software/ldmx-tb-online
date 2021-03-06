#ifndef EUDAQ_TESTBEAMEVENTDISPLAYMONITOR_H
#define EUDAQ_TESTBEAMEVENTDISPLAYMONITOR_H

//---< eudaq >---//
#include "eudaq/ROOTMonitor.hh"

#include "TH2.h"

namespace eudaq {

class TestBeamEventDisplayMonitor : public eudaq::ROOTMonitor {
public:
  TestBeamEventDisplayMonitor(const std::string &name, const std::string &runcontrol)
    : eudaq::ROOTMonitor(name, "TestBeamEventDisplayMonitor", runcontrol) {}

  ~TestBeamEventDisplayMonitor() = default;

  void AtConfiguration() override; 
  void AtEventReception(eudaq::EventSP ev) override; 

  static const uint32_t factory_id_{eudaq::cstr2hash("TestBeamEventDisplayMonitor")};
  
private:
  int getField(int value, int high_bit, int low_bit) {
    int mask{static_cast<int>(pow(2, (high_bit - low_bit + 1)) - 1)};
    return (value >> low_bit) & mask;
  }
  
  TH2D* hcal_event; 

  std::map<std::string, int> cmb_map;
  std::map<std::string, int> quadbar_map;
  std::map<std::string, int> bar_map;
  std::map<std::string, int> plane_map;

  std::map<std::string, int> detid_map;
  std::map<std::string, double> adcped_map;
  std::map<std::string, double> adcgain_map;
  std::map<std::string, double> totped_map;
  std::map<std::string, double> totgain_map;
  
  std::vector<int> unusedchans;
  int block_;
  int nPlanes;
  int nevents_reset;
  int nevents;
  
  double energy_per_mip; //MeV/MIP
  double voltage_hcal; //mV/PE
  double PE_per_mip; //PEs/mip
  double mV_per_PE; //mV per MIP is about 73 for now
};
} // namespace eudaq
#endif // EUDAQ_TESTBEAMEVENTDISPLAYMONITOR_H
