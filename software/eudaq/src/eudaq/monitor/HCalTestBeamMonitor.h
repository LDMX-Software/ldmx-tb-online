#ifndef EUDAQ_HCALTESTBEAMMONITOR_H
#define EUDAQ_HCALTESTBEAMMONITOR_H

//---< eudaq >---//
#include "eudaq/ROOTMonitor.hh"

#include "TH2.h"

#include "TH1.h"

#include "eudaq/CSVParser.h"

namespace eudaq {

class HCalTestBeamMonitor : public eudaq::ROOTMonitor {
public:
  HCalTestBeamMonitor(const std::string &name, const std::string &runcontrol)
    : eudaq::ROOTMonitor(name, "HCalTestBeamMonitor", runcontrol) {}

  ~HCalTestBeamMonitor() = default;

  void AtConfiguration() override; 
  void AtEventReception(eudaq::EventSP ev) override; 

  static const uint32_t factory_id_{eudaq::cstr2hash("HCalTestBeamMonitor")};
  
private:
  int getField(int value, int high_bit, int low_bit) {
    int mask{static_cast<int>(pow(2, (high_bit - low_bit + 1)) - 1)};
    return (value >> low_bit) & mask;
  }

  std::map<std::string, TH2D*> adc_histo_map;
  std::map<std::string, TH2D*> tot_histo_map;
  std::map<std::string, TH2D*> toa_histo_map; 
  TH2D* hcalhits_top; 
  TH2D* hcalhits_bot; 
  TH1D* total_energy; 
  std::map<std::string, int> cmb_map;
  std::map<std::string, int> quadbar_map;
  std::map<std::string, int> bar_map;
  std::map<std::string, int> plane_map;

  std::map<std::string, int> detid_map;
  std::map<std::string, double> adcped_map;
  std::map<std::string, double> adcgain_map;
  std::map<std::string, double> totped_map;
  std::map<std::string, double> totgain_map;

  int nPlanes;
  int hcal_adc_thresh;
  double threshold_PE;
  std::vector<int> unusedchans;
};
} // namespace eudaq
#endif // EUDAQ_HCALTESTBEAMMONITOR_H
