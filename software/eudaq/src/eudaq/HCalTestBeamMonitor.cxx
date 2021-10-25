#include "eudaq/HCalTestBeamMonitor.h"

#include <iostream>

#include "eudaq/HgcrocDataPacket.h"

namespace {
auto dummy0 = eudaq::Factory<eudaq::Monitor>::Register<
    eudaq::HCalTestBeamMonitor, const std::string &, const std::string &>(
    eudaq::HCalTestBeamMonitor::factory_id_);
}

namespace eudaq {

void HCalTestBeamMonitor::AtConfiguration() {
  nPlanes = 19;
  // TODO Use DAQ map to fill map
  for (int i{256}; i < 264; ++i) {
    histo_map["ROC " + std::to_string(i) + " - ADC"] =
        m_monitor->Book<TH2D>("ROC " + std::to_string(i) + " ADC",
                              "ROC_" + std::to_string(i) + "_ADC", "",
                              ";Channel;ADC", 40, 0, 40, 200, 0, 200);
    m_monitor->SetDrawOptions(histo_map["ROC " + std::to_string(i) + " - ADC"],
                              "colz");
  }
  for (int i{1}; i < nPlanes + 1; i++){
    if(nPlanes < 10){
      if(i%2 != 0){
        hcal_run_map[i] = m_monitor->Book<TH2D>("Run/Plane " + std::to_string(i), "Plane " + std::to_string(i), "", ";Bar End;Channel", 8, 0, 8, 2, 0, 2);
      } else{
        hcal_run_map[i] = m_monitor->Book<TH2D>("Run/Plane " + std::to_string(i), "Plane " + std::to_string(i), "", ";Bar End;Channel", 2, 0, 2, 8, 0, 8);
      }
    } else{
      if(i%2 != 0){
        hcal_run_map[i] = m_monitor->Book<TH2D>("Run/Plane " + std::to_string(i), "Plane " + std::to_string(i), "", ";Bar End;Channel", 12, 0, 12, 2, 0, 2);
      } else{
        hcal_run_map[i] = m_monitor->Book<TH2D>("Run/Plane " + std::to_string(i), "Plane " + std::to_string(i), "", ";Bar End;Channel", 2, 0, 2, 12, 0, 12);
    }
    m_monitor->SetDrawOptions(hcal_run_map[i], "colz");
  }
  }
  daq_map = getDaqMap("/home/ldmx/OnlineMonitoring_test/hcal_map.csv");
  ped_map = getPedMap("/home/ldmx/OnlineMonitoring_test/DumbReconConditions.csv");
}

void HCalTestBeamMonitor::AtEventReception(EventSP event) {
  // Fill HCal plots
  auto hcal_event{std::make_shared<HgcrocDataPacket>(*event)};
  
  auto thresh_map{std::map<std::string, int>()};

  auto samples{hcal_event->getSamples()};
  for (auto &sample : samples) {
    for (auto &subpacket : sample.subpackets) {
      auto roc_id{subpacket.roc_id};
      for (int i{0}; i < subpacket.adc.size(); ++i) {
        histo_map["ROC " + std::to_string(roc_id) + " - ADC"]->Fill(
            i, subpacket.adc[i]);
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
        if(!hcal_run_map.count(plane)){
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
            hcal_run_map[plane]->Fill(chan, 1 - end);
          } else{
            hcal_run_map[plane]->Fill(end, chan);
          }
        }
      }
    }
  }
}
} // namespace eudaq
