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
  hcalhits_top = m_monitor->Book<TH2D>("hcalhits_top", "hcalhits_top", "", ";Plane;Bar", nPlanes+1, 0, nPlanes+1, 12, 0, 12);
  hcalhits_bot = m_monitor->Book<TH2D>("hcalhits_bot", "hcalhits_bot", "", ";Plane;Bar", nPlanes+1, 0, nPlanes+1, 12, 0, 12);
  m_monitor->SetDrawOptions(hcalhits_top, "colz");
  m_monitor->SetDrawOptions(hcalhits_bot, "colz");
  hcalhits_top->SetTitle("Hits Above Threshold Top/Right");
  hcalhits_bot->SetTitle("Hits Above Threshold Bot/Left");
  hcalhits_top->SetStats(0);
  hcalhits_bot->SetStats(0);
  total_energy = m_monitor->Book<TH1D>("total_energy", "total_energy", "", ";Energy per Event;", 100, 0, 1000);
  total_energy->SetTitle("Total Energy Deposited per Event");
  total_energy->SetStats(0);
  std::string daqpath = std::getenv("DAQ_INSTALL_PREFIX");
  auto daqmapfile{conf->Get("HCALDAQMAP", "")};
  auto gainfile{conf->Get("HCALGAIN", "")};
  std::string fulldaqfile = daqpath + "/" + daqmapfile;
  EUDAQ_INFO("Reading HCal DAQ map from " + fulldaqfile);
  std::ifstream indaq(fulldaqfile.c_str());
  if (!indaq.is_open()){
    EUDAQ_THROW("Failed to open HCal DAQ map file " + fulldaqfile);
  }
  std::string fullgainfile = daqpath + "/" + gainfile;
  EUDAQ_INFO("Reading HCal gains from " + fullgainfile);
  std::ifstream ingain(fullgainfile.c_str());
  if (!ingain.is_open()){
    EUDAQ_THROW("Failed to open HCal gain file " + fullgainfile);
  }
  cmb_map = CSVParser::getCMBMap(fulldaqfile);
  quadbar_map = CSVParser::getQuadbarMap(fulldaqfile);
  bar_map = CSVParser::getBarMap(fulldaqfile);
  plane_map = CSVParser::getPlaneMap(fulldaqfile);
  detid_map = CSVParser::getDetIDMap(fullgainfile);
  adcped_map = CSVParser::getADCPedMap(fullgainfile);
  adcgain_map = CSVParser::getADCGainMap(fullgainfile);
  totped_map = CSVParser::getTOTPedMap(fullgainfile);
  totgain_map = CSVParser::getTOTGainMap(fullgainfile);
  
  unusedchans = {8, 17, 26, 35, 44, 53, 62, 71, 72};
  threshold_PE = 5.;
}

void HCalTestBeamMonitor::AtEventReception(EventSP event) {
  // Fill HCal plots
  auto data = hcal::decode(event->GetBlock(1));
  
  double energy = 0;

  for (const auto& [el, samples] : data) {
    int fpga = el.fpga();
    int roc = el.roc();
    int channel = el.channel();
    int link = el.link();
    int inlink = el.inlink_channel();
    if(std::count(unusedchans.begin(), unusedchans.end(), channel)) continue;
    //std::cout<<"fpga: "<<fpga<<"  roc: "<<roc<<"  channel: "<<channel<<"  link: "<<link<<"  inlink: "<<inlink<<"  sample size: "<<samples.size()<<std::endl;
    std::string rocchan = std::to_string(roc+1) + ":" + std::to_string(channel);
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
    int detid = -9999;
    double adcped = -9999.;
    double adcgain = -9999.;
    double totped = -9999.;
    double totgain = -9999.;
    if(detid_map.count(rocchan) > 0 && adcped_map.count(rocchan) > 0 && adcgain_map.count(rocchan) > 0 && totped_map.count(rocchan) > 0 && totgain_map.count(rocchan) > 0){
      detid = detid_map.at(rocchan);
      adcped = adcped_map.at(rocchan);
      adcgain = adcgain_map.at(rocchan);
      totped = totped_map.at(rocchan);
      totgain = totgain_map.at(rocchan);
    }
    else{
      std::cout << "Key not found: " << rocchan << std::endl;
    }
    double maxadc = -9999.;
    double threshold = 0; //adcped + adcgain; //This is wrong fix it
    for (auto &sample : samples) {
      bool isTOT = sample.isTOTinProgress();
      bool isTOTComplete = sample.isTOTComplete();
      int toa = sample.toa();
      int tot = sample.tot();
      int adc_tm1 = sample.adc_tm1(); //not really used without zero suppression
      int adc_t = sample.adc_t();
      if(adc_t > maxadc) maxadc = adc_t;
      adc_histo_map["ROC " + std::to_string(roc) + " - ADC"]->Fill(channel, adc_t);
      tot_histo_map["ROC " + std::to_string(roc) + " - TOT"]->Fill(channel, tot);
      toa_histo_map["ROC " + std::to_string(roc) + " - TOA"]->Fill(channel, toa);
      //std::cout<<"isTOT: "<<isTOT<<"  isTOTComplete: "<<isTOTComplete<<"  toa: "<<toa<<"  tot: "<<tot<<"  adc_tm1: "<<adc_tm1<<"  adc_t "<<adc_t<<std::endl;
    }
    if(maxadc >= threshold){
      if(end != 0){
	hcalhits_top->Fill(plane, barchan);
      }
      else{
        hcalhits_bot->Fill(plane, barchan);
      }
    }
    double energy_chan = (maxadc - adcped) * adcgain; //this is wrong. fix it.
    energy = energy + energy_chan;
  }
  total_energy->Fill(energy);
}
} // namespace eudaq
