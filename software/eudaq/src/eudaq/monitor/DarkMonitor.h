#ifndef EUDAQ_SIMPLEMONITOR_H
#define EUDAQ_SIMPLEMONITOR_H

//---< eudaq >---//
#include "eudaq/ROOTMonitor.hh"
#include "TH1.h"
#include "TH2.h"

namespace eudaq {

class DarkMonitor : public eudaq::ROOTMonitor {
public:
  DarkMonitor(const std::string &name, const std::string &runcontrol)
    : eudaq::ROOTMonitor(name, "DarkMonitor", runcontrol) {}

  ~DarkMonitor() = default;

  void AtConfiguration() override;
  void AtEventReception(eudaq::EventSP ev) override;

  static const uint32_t factory_id_{eudaq::cstr2hash("DarkMonitor")};
private:
  int getField(int value, int high_bit, int low_bit) {
    int mask{static_cast<int>(pow(2, (high_bit - low_bit + 1)) - 1)};
    return (value >> low_bit) & mask;
  }

  TH2D* hcalRunPlane0;
  TH2D* hcalRunPlane1;
  TH2D* hcalRunPlane2;
  TH2D* hcalRunPlane3;
  TH2D* hcalRunPlane4;
  TH2D* hcalRunPlane5;
  TH2D* hcalRunPlane6;
  TH2D* hcalRunPlane7;
  TH2D* hcalRunPlane8;
  TH2D* hcalRunPlane9;
  TH2D* hcalRunPlane10;
  TH2D* hcalRunPlane11;
  TH2D* hcalRunPlane12;
  TH2D* hcalRunPlane13;
  TH2D* hcalRunPlane14;
  TH2D* hcalRunPlane15;
  TH2D* hcalRunPlane16;
  TH2D* hcalRunPlane17;
  TH2D* hcalRunPlane18;
  TH2D* hcalEventPlane0;
  TH2D* hcalEventPlane1;
  TH2D* hcalEventPlane2;
  TH2D* hcalEventPlane3;
  TH2D* hcalEventPlane4;
  TH2D* hcalEventPlane5;
  TH2D* hcalEventPlane6;
  TH2D* hcalEventPlane7;
  TH2D* hcalEventPlane8;
  TH2D* hcalEventPlane9;
  TH2D* hcalEventPlane10;
  TH2D* hcalEventPlane11;
  TH2D* hcalEventPlane12;
  TH2D* hcalEventPlane13;
  TH2D* hcalEventPlane14;
  TH2D* hcalEventPlane15;
  TH2D* hcalEventPlane16;
  TH2D* hcalEventPlane17;
  TH2D* hcalEventPlane18;
  TH2D* tsRun;
  TH2D* tsEvent;
  
  int eventNum;
  int adc_thesh;
};
} // namespace eudaq
#endif // EUDAQ_DARKMONITOR_H
