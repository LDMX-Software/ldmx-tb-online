#include "eudaq/TestBeamMonitor.h"

#include <iostream>

#include "eudaq/HgcrocDataPacket.h"

namespace {
auto dummy0 = eudaq::Factory<eudaq::Monitor>::Register<
    eudaq::TestBeamMonitor, const std::string &, const std::string &>(
    eudaq::TestBeamMonitor::factory_id_);
}

namespace eudaq {

void TestBeamMonitor::AtConfiguration() {
  nPlanes = 19;
  hcal_adc_thresh = 0; //adjust later
  // TODO Use DAQ map to fill map
  for (int i{256}; i < 264; ++i) {
    histo_map["ROC " + std::to_string(i) + " - ADC"] = m_monitor->Book<TH2D>(
        "ROC " + std::to_string(i) + " ADC", "ROC_" + std::to_string(i) + "_ADC", 
	"", ";Channel;ADC",
        40, 0, 40, 200, 0, 200);
    m_monitor->SetDrawOptions(histo_map["ROC " + std::to_string(i) + " - ADC"],
                              "colz");
  }
  for (int i{1}; i < nPlanes + 1; i++){
    if(nPlanes < 10){
      if(i%2 != 0){
        hcal_run_map[i] = m_monitor->Book<TH2D>("Run/Plane " + std::to_string(i), "Plane " + std::to_string(i), "", ";Bar End;Channel", 16, 0, 16, 2, 0, 2);
        hcal_event_map[i] = m_monitor->Book<TH2D>("Event/Plane " + std::to_string(i), "Plane " + std::to_string(i), "", ";Bar End;Channel", 16, 0, 16, 2, 0, 2);
      } else{
        hcal_run_map[i] = m_monitor->Book<TH2D>("Run/Plane " + std::to_string(i), "Plane " + std::to_string(i), "", ";Bar End;Channel", 2, 0, 2, 16, 0, 16);
        hcal_event_map[i] = m_monitor->Book<TH2D>("Event/Plane " + std::to_string(i), "Plane " + std::to_string(i), "", ";Bar End;Channel", 2, 0, 2, 16, 0, 16);
      }
    } else{
      if(i%2 != 0){
        hcal_run_map[i] = m_monitor->Book<TH2D>("Run/Plane " + std::to_string(i), "Plane " + std::to_string(i), "", ";Bar End;Channel", 24, 0, 24, 2, 0, 2);
        hcal_event_map[i] = m_monitor->Book<TH2D>("Event/Plane " + std::to_string(i), "Plane " + std::to_string(i), "", ";Bar End;Channel", 24, 0, 24, 2, 0, 2);
      } else{
        hcal_run_map[i] = m_monitor->Book<TH2D>("Run/Plane " + std::to_string(i), "Plane " + std::to_string(i), "", ";Bar End;Channel", 2, 0, 2, 24, 0, 24);
        hcal_event_map[i] = m_monitor->Book<TH2D>("Event/Plane " + std::to_string(i), "Plane " + std::to_string(i), "", ";Bar End;Channel", 2, 0, 2, 24, 0, 24);
    }
    m_monitor->SetDrawOptions(hcal_run_map[i], "colz");
    m_monitor->SetDrawOptions(hcal_event_map[i], "colz");
  }
  }
  ts_run = m_monitor->Book<TH2D>("Run/TrigScint", "TrigScint", "", ";Channel; ", 1, 0, 1, 12, 0, 12);
  ts_event = m_monitor->Book<TH2D>("Event/TrigScint", "TrigScint", "", ";Channel; ", 1, 0, 1, 12, 0, 12);
  m_monitor->SetDrawOptions(ts_run, "colz");
  m_monitor->SetDrawOptions(ts_event, "colz");
  daq_map = getDaqMap("/home/ldmx/OnlineMonitoring_test/hcal_map.csv");
}

void TestBeamMonitor::AtEventReception(EventSP event) {

  bool newSpill = false; //TODO Set new spill flag 
  //Reset event plots if new spill
  if(newSpill){
    for (int i{1}; i < nPlanes + 1; i++){
      //hcal_event_map[i].Reset();
    }
    //ts_event.Reset();
  }
  // Fill HCal plots
  auto hcal_event{std::make_shared<HgcrocDataPacket>(*event)};

  auto subpackets{hcal_event->getSubpackets()};
  for (auto &subpacket : subpackets) {
    auto roc_id{subpacket.roc_id};
    std::cout << "Samples: " << subpacket.adc.size() << std::endl;
    for (int i{0}; i < subpacket.adc.size(); ++i) {
      histo_map["ROC " + std::to_string(roc_id) + " - ADC"]->Fill(
          i, subpacket.adc[i]);
      if(subpacket.adc[i] >= hcal_adc_thresh){
          std::string channel = std::to_string(roc_id) + "," + std::to_string(i);
          int plane = getPlane(daq_map[channel]);
          int cmb = getCMB(daq_map[channel]);
          int quadbar = getQuadBar(daq_map[channel]);
          int bar = getBar(daq_map[channel]);
          int end = cmb%2;
          int chan = (bar - 1) + (quadbar - 1) * 4;
          if(plane%2 != 0){
            hcal_run_map[plane]->Fill(chan, end);
            hcal_event_map[plane]->Fill(chan, end);
          } else{
            hcal_run_map[plane]->Fill(end, chan);
            hcal_event_map[plane]->Fill(end, chan);
          }
      }
    }
  }
  
  //TODO Fill TS Plots
  ts_run->Fill(0., 0.);
  ts_event->Fill(0., 0.);
  // if (event->GetDescription().compare("HCalTestBeamRaw") == 0) {
  //}
}
} // namespace eudaq
