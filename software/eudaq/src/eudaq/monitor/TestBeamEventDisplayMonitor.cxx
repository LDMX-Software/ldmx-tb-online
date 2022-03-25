#include "TestBeamEventDisplayMonitor.h"

#include <iostream>

#include "eudaq/HgcrocRawDecode.h"

#include "eudaq/CSVParser.h"

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
  std::string daqmapfile = "~/OnlineMonitor/ldmx-tb-online/software/data/testbeam_connections.csv";
  std::string gainfile = "~/OnlineMonitor/ldmx-tb-online/software/data/testbeam_connections.csv";
  cmb_map = CSVParser::getCMBMap(daqmapfile);
  quadbar_map = CSVParser::getQuadbarMap(daqmapfile);
  bar_map = CSVParser::getBarMap(daqmapfile);
  plane_map = CSVParser::getPlaneMap(daqmapfile);
  detid_map = CSVParser::getDetIDMap(gainfile);
  adcped_map = CSVParser::getADCPedMap(gainfile);
  adcgain_map = CSVParser::getADCGainMap(gainfile);
  totped_map = CSVParser::getTOTPedMap(gainfile);
  totgain_map = CSVParser::getTOTGainMap(gainfile);
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
        std::cout<<subpacket.roc_id<<"  "<<i<<"  "<<subpacket.adc[i]<<std::endl;
        if(i >= 32){
          continue;
        }

	std::string rocchan = std::to_string(roc_id+1) + "," + std::to_string(i);
	int cmb = cmb_map.at(rocchan);
	int quadbar = quadbar_map.at(rocchan);
	int bar = bar_map.at(rocchan);
	int plane = plane_map.at(rocchan);
	int end = cmb%2;
	std::string digiid = "HcalDigiID(0:" + std::to_string(plane) + ":" + std::to_string((4 - bar) + (quadbar - 1) * 4) + ":" + std::to_string(end) + ")";
	int chan = (quadbar - 1) * 4 + (4 - bar) * end + (1 - bar) * (end - 1);	
	int detid = detid_map.at(digiid);
	double adcped = adcped_map.at(digiid);
	double adcgain = adcgain_map.at(digiid);
	double totped = totped_map.at(digiid);
	double totgain = totgain_map.at(digiid);

	double thresh = adcped + adcgain;

        if(subpacket.adc[i] >= thresh && !thresh_map.count(rocchan)){
          thresh_map.insert(std::pair<std::string, int>(rocchan, 1));
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
