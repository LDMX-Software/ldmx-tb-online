#include "TestBeamEventDisplayMonitor.h"

#include <iostream>

#include "eudaq/HgcrocDataPacket.h"

namespace {
auto dummy0 = eudaq::Factory<eudaq::Monitor>::Register<
    eudaq::TestBeamEventDisplayMonitor, const std::string &, const std::string &>(
    eudaq::TestBeamEventDisplayMonitor::factory_id_);
}

namespace eudaq {

void TestBeamEventDisplayMonitor::AtConfiguration() {
  nPlanes = 19;
  time_reset = 18; //seconds
  n = 0;

  for (int i{1}; i < nPlanes + 1; i++){
    if(nPlanes < 10){
      if(i%2 != 0){
        hcal_event_map[i] = m_monitor->Book<TH2D>("Event/Plane " + std::to_string(i), "Plane " + std::to_string(i), "", ";Bar End;Channel", 8, 0, 8, 2, 0, 2);
      } else{
        hcal_event_map[i] = m_monitor->Book<TH2D>("Event/Plane " + std::to_string(i), "Plane " + std::to_string(i), "", ";Bar End;Channel", 2, 0, 2, 8, 0, 8);
      }
    } else{
      if(i%2 != 0){
        hcal_event_map[i] = m_monitor->Book<TH2D>("Event/Plane " + std::to_string(i), "Plane " + std::to_string(i), "", ";Bar End;Channel", 12, 0, 12, 2, 0, 2);
      } else{
        hcal_event_map[i] = m_monitor->Book<TH2D>("Event/Plane " + std::to_string(i), "Plane " + std::to_string(i), "", ";Bar End;Channel", 2, 0, 2, 12, 0, 12);
    }
    m_monitor->SetDrawOptions(hcal_event_map[i], "colz");
  }
  }
  ts_event = m_monitor->Book<TH2D>("Event/TrigScint", "TrigScint", "", ";Channel; ", 1, 0, 1, 12, 0, 12);
  m_monitor->SetDrawOptions(ts_event, "colz");
  daq_map = getDaqMap("/home/ldmx/OnlineMonitoring_test/hcal_map.csv");
  ped_map = getPedMap("/home/ldmx/OnlineMonitoring_test/DumbReconConditions.csv");
}

void TestBeamEventDisplayMonitor::AtEventReception(EventSP event) {

  //time = time;
  //bool newSpill = time - prev_time > time_reset; //reset plots every 18 s for now
  
  bool newSpill = n%100 == 0;
  
  //Reset event plots if new spill
  if(!newSpill){
    return;
  }
  else{
    for (int i{1}; i < nPlanes + 1; i++){
      hcal_event_map[i]->Reset();
    }
    ts_event->Reset();
    //previous_time = time;
  }
  // Fill HCal plots
  auto hcal_event{std::make_shared<HgcrocDataPacket>(*event)};
  
  auto samples{hcal_event->getSamples()};
  
  auto thresh_map{std::map<std::string, int>()};
  
  for (auto &sample : samples) {
    for (auto &subpacket : sample.subpackets) {
      auto roc_id{subpacket.roc_id};
      std::cout << "Samples: " << subpacket.adc.size() << std::endl;
      for (int i{0}; i < subpacket.adc.size(); ++i) {
        if(i >= 32){
          continue;
        }
        int roc_num = (int) (round((roc_id - 256) / 2) + 1);
        int chan_num = i + 32 * (roc_id%2);
        std::string channel = std::to_string(roc_num) + "," + std::to_string(chan_num);
        if(!daq_map.count(channel)){
          std::cout<<"No map found " << channel << std::endl;
          continue;
        }
        int plane = getPlane(daq_map[channel]);
        if(!hcal_event_map.count(plane)){
          std::cout<< "Histo for plane " << std::to_string(plane) << " not found" << std::endl;
          continue;
        }
        int cmb = getCMB(daq_map[channel]);
        int quadbar = getQuadBar(daq_map[channel]);
        int bar = getBar(daq_map[channel]);
        int end = cmb%2;
        int chan = (quadbar - 1) * 4 + (4 - bar) * end + (1 - bar) * (end - 1);
        std::string pedchan = "HcalDigiID(0:" + std::to_string(plane) + ":" + std::to_string((4 - bar) + (quadbar - 1) * 4) + ":" + std::to_string(end) + ")";
        if(!ped_map.count(pedchan)){
          std::cout<< "Threshold/Pedestal Map for " << pedchan << " not found" << std::endl;
          continue;
        }
        int pedestal = getPedest(ped_map[pedchan]);
        int threshold = getThresh(ped_map[pedchan]);
        int thresh = pedestal + threshold;
        if(subpacket.adc[i] >= thresh && !thresh_map.count(channel)){
          thresh_map.insert(std::pair<std::string, int>(channel, 1));
          if(plane%2 != 0){
            hcal_event_map[plane]->Fill(chan, 1 - end);
          } else{
            hcal_event_map[plane]->Fill(end, chan);
          }
      }
    }
  }
  }

  //TODO Fill TS Plots
  ts_event->Fill(0., 0.);
}
} // namespace eudaq
