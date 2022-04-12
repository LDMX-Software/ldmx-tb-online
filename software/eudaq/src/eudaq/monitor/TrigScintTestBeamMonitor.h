#ifndef EUDAQ_TRIGSCINTTESTBEAMMONITOR_H
#define EUDAQ_TRIGSCINTTESTBEAMMONITOR_H

//---< eudaq >---//
#include "eudaq/ROOTMonitor.hh"
#include "eudaq/SimQIE.h"
#include "TH2.h"

namespace eudaq {

static const unsigned int n_channels=16;

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
  static const unsigned int comma_char=0xF7FB;
  SimQIE qie_converter;
  std::vector<uint16_t> event_buffer1,event_buffer2;
  //std::map<std::string, TH2D*> histo_map;

  // List of histograms to plot.
  std::vector<TH2F*> h2_ADCvT;			// histograms of ADC vs time sample
  std::vector<TH2F*> h2_QvT;			// histograms of Q vs time sample
  std::vector<TH2F*> h2_TDCvT;			// histograms of TDC vs time sample
  std::vector<TH1F*> h1_ADC;			// histograms of ADCs
  std::vector<TH1F*> h1_sum_Q;			// histograms of Q summed over all time samples
  std::vector<TH1F*> h1_Q;			// histograms of Q
  std::vector<TH1F*> h1_TDC;			// histograms of TDCs
  //TH1F* h1_CID1;        			// histogram of CIDs for fiber 1
  //TH1F* h1_CID2;	         		// histogram of CIDs for fiber 2
  //TH1F* h1_time;         			// histogram of trigger times
  
};
} // namespace eudaq
#endif // EUDAQ_TRIGSCINTTESTBEAMMONITOR_H
