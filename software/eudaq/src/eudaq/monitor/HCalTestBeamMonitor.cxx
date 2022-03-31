#include "HCalTestBeamMonitor.h"

#include <iostream>

#include "eudaq/HgcrocRawDecode.h"

#include "eudaq/CSVParser.h"

namespace {
auto dummy0 = eudaq::Factory<eudaq::Monitor>::Register<
    eudaq::HCalTestBeamMonitor, const std::string &, const std::string &>(
    eudaq::HCalTestBeamMonitor::factory_id_);
}

namespace eudaq {

void HCalTestBeamMonitor::AtConfiguration() {
  auto conf{GetConfiguration()};

  for (int i{0}; i < 6; ++i) {
    adc_histo_map["ROC " + std::to_string(i) + " - ADC"] =
        m_monitor->Book<TH2D>("ROC " + std::to_string(i) + " ADC",
                              "ROC_" + std::to_string(i) + "_ADC", "",
                              ";Channel;ADC", 72, 0, 72, 1000, 0, 1000);
    m_monitor->SetDrawOptions(adc_histo_map["ROC " + std::to_string(i) + " - ADC"], "colz");
    adc_histo_map["ROC " + std::to_string(i) + " - ADC"]->SetTitle(("ADC vs Channel, ROC " + std::to_string(i)).c_str());
    adc_histo_map["ROC " + std::to_string(i) + " - ADC"]->SetStats(0);   
    
    tot_histo_map["ROC " + std::to_string(i) + " - TOT"] =
        m_monitor->Book<TH2D>("ROC " + std::to_string(i) + " TOT",
                              "ROC_" + std::to_string(i) + "_TOT", "",
                              ";Channel;TOT", 72, 0, 72, 1000, 0, 1000);
    m_monitor->SetDrawOptions(tot_histo_map["ROC " + std::to_string(i) + " - TOT"], "colz");
    tot_histo_map["ROC " + std::to_string(i) + " - TOT"]->SetTitle(("TOT vs Channel, ROC " + std::to_string(i)).c_str());
    tot_histo_map["ROC " + std::to_string(i) + " - TOT"]->SetStats(0);   
    
    toa_histo_map["ROC " + std::to_string(i) + " - TOA"] =
        m_monitor->Book<TH2D>("ROC " + std::to_string(i) + " TOA",
                              "ROC_" + std::to_string(i) + "_TOA", "",
                              ";Channel;TOA", 72, 0, 72, 1000, 0, 1000);
    m_monitor->SetDrawOptions(toa_histo_map["ROC " + std::to_string(i) + " - TOA"], "colz");
    toa_histo_map["ROC " + std::to_string(i) + " - TOA"]->SetTitle(("TOA vs Channel, ROC " + std::to_string(i)).c_str());
    toa_histo_map["ROC " + std::to_string(i) + " - TOA"]->SetStats(0);                         
  }

  nPlanes = 19;
  hcalhits_top = m_monitor->Book<TH2D>("hcalhits_top", "hcalhits_top", "", ";Plane;Bar", nPlanes, 0, nPlanes, 12, 0, 12);
  hcalhits_bot = m_monitor->Book<TH2D>("hcalhits_bot", "hcalhits_bot", "", ";Plane;Bar", nPlanes, 0, nPlanes, 12, 0, 12);
  m_monitor->SetDrawOptions(hcalhits_top, "colz");
  m_monitor->SetDrawOptions(hcalhits_bot, "colz");
  hcalhits_top->SetTitle("Hits Above Threshold Top/Left");
  hcalhits_bot->SetTitle("Hits Above Threshold Bot/Right");
  hcalhits_top->SetStats(0);
  hcalhits_bot->SetStats(0);
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
  
  unusedchans = {8, 17, 26, 35, 44, 53, 62, 71, 72};
  threshold_PE = 5.;
}

void HCalTestBeamMonitor::AtEventReception(EventSP event) {
  // Fill HCal plots
  auto data = hcal::decode(event->GetBlock(1));

  auto thresh_map{std::map<std::string, int>()};

  for (const auto& [el, samples] : data) {
    int fpga = el.fpga();
    int roc = el.roc();
    int channel = el.channel();
    int link = el.link();
    int inlink = el.inlink_channel();
    if(std::count(unusedchans.begin(), unusedchans.end(), channel)) continue;
    std::cout<<"fpga: "<<fpga<<"  roc: "<<roc<<"  channel: "<<channel<<"  link: "<<link<<"  inlink: "<<inlink<<"  sample size: "<<samples.size()<<std::endl;
    std::string rocchan = std::to_string(roc+1) + "," + std::to_string(channel);
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
    for (auto &sample : samples) {
      bool isTOT = sample.isTOTinProgress();
      bool isTOTComplete = sample.isTOTComplete();
      int toa = sample.toa();
      int tot = sample.tot();
      int adc_tm1 = sample.adc_tm1(); //not really used without zero suppression
      int adc_t = sample.adc_t();
      adc_histo_map["ROC " + std::to_string(roc) + " - ADC"]->Fill(channel, adc_t);
      tot_histo_map["ROC " + std::to_string(roc) + " - TOT"]->Fill(channel, tot);
      toa_histo_map["ROC " + std::to_string(roc) + " - TOA"]->Fill(channel, toa);
      if(adc_t >= threshold && !thresh_map.count(rocchan)){
	  thresh_map.insert(std::pair<std::string, int>(rocchan, 1));
	  if(end == 0){
	    hcalhits_top->Fill(plane, barchan);
	  }
	  else{
            hcalhits_bot->Fill(plane, barchan);
          }
	}
      std::cout<<"isTOT: "<<isTOT<<"  isTOTComplete: "<<isTOTComplete<<"  toa: "<<toa<<"  tot: "<<tot<<"  adc_tm1: "<<adc_tm1<<"  adc_t "<<adc_t<<std::endl;
    }
  }
}
} // namespace eudaq
