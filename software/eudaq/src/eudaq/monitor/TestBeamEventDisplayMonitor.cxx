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
  auto conf{GetConfiguration()};
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

  auto daqmapfile{conf->Get("HCALDAQMAP", "")};
  EUDAQ_INFO("Reading HCal DAQ map from " + daqmapfile);
  auto gainfile{conf->Get("HCALGAIN", "")};
  EUDAQ_INFO("Reading HCal gains from " + gainfile);
  std::ifstream indaq(daqmapfile.c_str());
  if (!indaq.is_open()){
    EUDAQ_THROW("Failed to open HCal DAQ map file " + daqmapfile);
  }
  std::ifstream ingain(gainfile.c_str());
  if (!ingain.is_open()){
    EUDAQ_THROW("Failed to open HCal gain file " + gainfile);
  }
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

  auto data = hcal::decode(event->GetBlock(1));

  auto thresh_map{std::map<std::string, int>()};

  for (const auto& [el, samples] : data) {
    int fpga = el.fpga();
    int roc = el.roc();
    int channel = el.channel();
    int link = el.link();
    int inlink = el.inlink_channel();
    std::cout<<"fpga: "<<fpga<<"  roc: "<<roc<<"  channel: "<<channel<<"  link: "<<link<<"  inlink: "<<inlink<<"  sample size: "<<samples.size()<<std::endl;
    for (auto &sample : samples) {
      bool isTOT = sample.isTOTinProgress();
      bool isTOTComplete = sample.isTOTComplete();
      int toa = sample.toa();
      int tot = sample.tot();
      int adc_tm1 = sample.adc_tm1();
      int adc_t = sample.adc_t();
      std::cout<<"isTOT: "<<isTOT<<"  isTOTComplete: "<<isTOTComplete<<"  toa: "<<toa<<"  tot: "<<tot<<"  adc_tm1: "<<adc_tm1<<"  adc_t "<<adc_t<<std::endl;
    }
  }
  /*auto hcal_event{std::make_shared<HgcrocDataPacket>(*event)};
  
  auto samples{hcal_event->getSamples()};
  
  auto thresh_map{std::map<std::string, int>()};
  
  for (auto &sample : samples) {
    for (auto &subpacket : sample.subpackets) {
      auto roc_id{subpacket.roc_id};
      std::cout << "Samples: " << subpacket.adc.size() << std::endl;
      for (int i{0}; i < subpacket.adc.size(); ++i) {
	int channel = i; //Double check this. Probably not correct
	std::string rocchan = std::to_string(roc_id+1) + "," + std::to_string(channel);
	int cmb = -9999;
	int quadbar = -9999;
	int bar = -9999;
	int plane = -9999;

	if(cmb_map.count(rocchan) > 0 && quadbar_map.count(rocchan) > 0 && bar_map.count(rocchan) > 0 && plane_map.count(rocchan) > 0){
	  cmb = cmb_map.at(rocchan);
	  quadbar = quadbar_map.at(rocchan);
	  bar = bar_map.at(rocchan);
	  plane = plane_map.at(rocchan);
	}
        else{
          std::cout << "Key not found: " << rocchan << std::endl;
        }

	int end = cmb%2;
	int barchan = (4 - bar) + (quadbar - 1) * 4;
	std::string digiid = "HcalDigiID(0:" + std::to_string(plane) + ":" + std::to_string(barchan) + ":" + std::to_string(end) + ")";
	int detid = -9999;
	double adcped = -9999.;
	double adcgain = -9999.;
	double totped = -9999.;
	double totgain = -9999.;
	if(detid_map.count(digiid) > 0 && adcped_map.count(digiid) > 0 && adcgain_map.count(digiid) > 0 && totped_map.count(digiid) > 0 && totgain_map.count(digiid) > 0){
          detid = detid_map.at(digiid);
	  adcped = adcped_map.at(digiid);
	  adcgain = adcgain_map.at(digiid);
	  totped = totped_map.at(digiid);
	  totgain = totgain_map.at(digiid);
        }
        else{
          std::cout << "Key not found: " << digiid << std::endl;
        }
	double threshold = 0; //adcped + adcgain; //This is wrong fix it

        if(subpacket.adc[i] >= threshold && !thresh_map.count(rocchan)){
          thresh_map.insert(std::pair<std::string, int>(rocchan, 1));
          if(plane%2 != 0){
            hcal_event_map[plane]->Fill(barchan, 1 - end);
          } else{
            hcal_event_map[plane]->Fill(end, barchan);
          }
      }
    }
  }
  }*/

  //TODO Fill TS Plots
  ts_event->Fill(0., 0.);
}
} // namespace eudaq
