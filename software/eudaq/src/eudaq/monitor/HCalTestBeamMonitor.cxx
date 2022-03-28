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
  // TODO Use DAQ map to fill map
  for (int i{0}; i < 6; ++i) {
    histo_map["ROC " + std::to_string(i) + " - ADC"] =
        m_monitor->Book<TH2D>("ROC " + std::to_string(i) + " ADC",
                              "ROC_" + std::to_string(i) + "_ADC", "",
                              ";Channel;ADC", 40, 0, 40, 200, 0, 200);
    m_monitor->SetDrawOptions(histo_map["ROC " + std::to_string(i) + " - ADC"],
                              "colz");
  }

  nPlanes = 19;
  hcalhits_top = m_monitor->Book<TH2D>("hcalhits_top", "hcalhits_top", "", ";Plane;Bar", nPlanes, 0, nPlanes, 12, 0, 12);
  hcalhits_bot = m_monitor->Book<TH2D>("hcalhits_bot", "hcalhits_bot", "", ";Plane;Bar", nPlanes, 0, nPlanes, 12, 0, 12);
  m_monitor->SetDrawOptions(hcalhits_top, "colz");
  m_monitor->SetDrawOptions(hcalhits_bot, "colz");
  std::string daqmapfile = "/home/mrsolt91/OnlineMonitor/ldmx-tb-online/software/data/testbeam_connections.csv";
  std::string gainfile = "/home/mrsolt91/OnlineMonitor/ldmx-tb-online/software/data/DumbReconConditions.csv";
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

void HCalTestBeamMonitor::AtEventReception(EventSP event) {
  // Fill HCal plots
  auto hcal_event{std::make_shared<HgcrocDataPacket>(*event)};

  auto samples{hcal_event->getSamples()};
  for (auto &sample : samples) {
    for (auto &subpacket : sample.subpackets) {
      auto roc_id{subpacket.roc_id};
      for (int i{0}; i < subpacket.adc.size(); ++i) {
        histo_map["ROC " + std::to_string(roc_id) + " - ADC"]->Fill(
            i, subpacket.adc[i]);
	int channel = i; //Double check this. Probably not correct
	int chip_channel = channel - (channel > 19)*1;
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
	std::string digiid = "HcalDigiID(0:" + std::to_string(plane) + ":" + std::to_string(barchan) + ":" + std::to_string(end) + ")";s
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
	if(subpacket.adc[i] > threshold){
	  if(end == 0){
	    hcalhits_top->Fill(plane, barchan);
	  }
	  else{
            hcalhits_bot->Fill(plane, barchan);
          }
	}
      }
    }
  }
}
} // namespace eudaq
