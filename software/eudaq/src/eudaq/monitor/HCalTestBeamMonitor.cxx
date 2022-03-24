#include "HCalTestBeamMonitor.h"

#include <iostream>

#include "eudaq/HgcrocDataPacket.h"

#include "eudaq/CSVParser.h"

namespace {
auto dummy0 = eudaq::Factory<eudaq::Monitor>::Register<
    eudaq::HCalTestBeamMonitor, const std::string &, const std::string &>(
    eudaq::HCalTestBeamMonitor::factory_id_);
}

namespace eudaq {

void HCalTestBeamMonitor::AtConfiguration() {
  // TODO Use DAQ map to fill map
  //for (int i{256}; i < 264; ++i) {
  for (int i{0}; i < 6; ++i) {
    histo_map["ROC " + std::to_string(i) + " - ADC"] =
        m_monitor->Book<TH2D>("ROC " + std::to_string(i) + " ADC",
                              "ROC_" + std::to_string(i) + "_ADC", "",
                              ";Channel;ADC", 40, 0, 40, 200, 0, 200);
    m_monitor->SetDrawOptions(histo_map["ROC " + std::to_string(i) + " - ADC"],
                              "colz");
  }

  nPlanes = 19;
  histo = m_monitor->Book<TH2D>("histo", "histo", "", ";Channel;ADC", nPlanes, 0, nPlanes, 12, 0, 12);
  m_monitor->SetDrawOptions(histo, "colz");
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
	std::cout<<subpacket.roc_id<<"  "<<i<<"  "<<subpacket.adc[i]<<std::endl;
	int channel = 1;
	std::string rocchan = std::to_string(roc_id+1) + "," + std::to_string(channel);
	int cmb = cmb_map.at(rocchan);
	int quadbar = quadbar_map.at(rocchan);
	int bar = bar_map.at(rocchan);
	int plane = plane_map.at(rocchan);
	int end = cmb%2;
	std::string digiid = "HcalDigiID(0:" + std::to_string(plane) + ":" + std::to_string((4 - bar) + (quadbar - 1) * 4) + ":" + std::to_string(end) + ")";
	int detid = detid_map.at(digiid);
	double adcped = adcped_map.at(digiid);
	double adcgain = adcgain_map.at(digiid);
	double totped = totped_map.at(digiid);
	double totgain = totgain_map.at(digiid);
	histo->Fill(plane, 1);
      }
    }
  }
}
} // namespace eudaq
