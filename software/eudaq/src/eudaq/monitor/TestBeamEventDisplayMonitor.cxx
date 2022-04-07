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
  nevents = 0;
  auto nreset{conf->Get("NRESET", "")};
  nevents_reset = std::stoi(nreset);

  hcal_event = m_monitor->Book<TH2D>("hcal_event", "hcal_event", "", ";Plane;Bar", nPlanes+1, 0, nPlanes+1, 12, 0, 12);
  m_monitor->SetDrawOptions(hcal_event, "colz");
  hcal_event->SetTitle("Hcal Event PEs per Bar");
  hcal_event->SetStats(0);

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
  block_ = conf->Get("FPGA_ID",0);
  
  unusedchans = {8, 17, 26, 35, 44, 53, 62, 71, 72};
  
  energy_per_mip = 4.66; //MeV/MIP
  voltage_hcal = 5.; //mV/PE
  PE_per_mip = 68.; //PEs/mip
  mV_per_PE = 1/energy_per_mip * voltage_hcal * PE_per_mip; //mV per MIP is about 73 for now
}

void TestBeamEventDisplayMonitor::AtEventReception(EventSP event) {
  
  bool newSpill = nevents%nevents_reset == 0;
  
  //Reset event plots if new spill
  if(!newSpill){
    return;
  }
  else{
    hcal_event->Reset();
  }

  auto data = hcal::decode(event->GetBlock(block_));

  auto thresh_map{std::map<std::string, int>()};
  
  std::map<std::string, double> physical_map;

  for (const auto& [el, samples] : data) {
    int fpga = el.fpga();
    int roc = el.roc();
    int channel = el.channel();
    int link = el.link();
    int inlink = el.inlink_channel();
    if(std::count(unusedchans.begin(), unusedchans.end(), channel)) continue;
    std::string rocchan = std::to_string(fpga)+":"+std::to_string(roc) + ":" + std::to_string(channel);
    //std::cout<<"fpga: "<<fpga<<"  roc: "<<roc<<"  channel: "<<channel<<"  link: "<<link<<"  inlink: "<<inlink<<"  sample size: "<<samples.size()<<std::endl;
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
    std::string location = std::to_string(plane) + ":" + std::to_string(barchan) + ":" + std::to_string(end);
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
    for (auto &sample : samples) {
      bool isTOT = sample.isTOTinProgress();
      bool isTOTComplete = sample.isTOTComplete();
      int toa = sample.toa();
      int tot = sample.tot();
      int adc_tm1 = sample.adc_tm1();
      int adc_t = sample.adc_t();
      if(adc_t > maxadc) maxadc = adc_t;
      //std::cout<<"isTOT: "<<isTOT<<"  isTOTComplete: "<<isTOTComplete<<"  toa: "<<toa<<"  tot: "<<tot<<"  adc_tm1: "<<adc_tm1<<"  adc_t "<<adc_t<<std::endl;
    }
    double PE_chan = (maxadc - adcped) / mV_per_PE * adcgain;
    if(PE_chan < 0) PE_chan = 0;
    physical_map.insert(std::pair<std::string, double>(location, PE_chan));   
  }
  for(int i = 1; i < nPlanes+1; i++){
    for(int j = 0; j < 12; j++){
      std::string key0 = std::to_string(i) + ":" + std::to_string(j) + ":0";
      std::string key1 = std::to_string(i) + ":" + std::to_string(j) + ":1";
      double PEsum = 0;
      if(physical_map.count(key0) > 0 && physical_map.count(key1) > 0){
        PEsum = physical_map.at(key0) + physical_map.at(key1);
      }
      hcal_event->Fill(i, j, PEsum);
    }
  }
  nevents++;
}
} // namespace eudaq
