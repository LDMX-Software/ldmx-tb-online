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
  // TODO Use DAQ map to fill map
  for (int i{256}; i < 264; ++i) {
    histo_map["ROC " + std::to_string(i) + " - ADC"] = m_monitor->Book<TH2D>(
        "ROC " + std::to_string(i) + " ADC", "ROC_" + std::to_string(i) + "_ADC", 
	"", ";Channel;ADC",
        40, 0, 40, 200, 0, 200);
    m_monitor->SetDrawOptions(histo_map["ROC " + std::to_string(i) + " - ADC"],
                              "colz");
  }
}

void TestBeamMonitor::AtEventReception(EventSP event) {
  // Fill HCal plots
  auto hcal_event{std::make_shared<HgcrocDataPacket>(*event)};

  auto subpackets{hcal_event->getSubpackets()};
  for (auto &subpacket : subpackets) {
    auto roc_id{subpacket.roc_id};
    for (int i{0}; i < subpacket.adc.size(); ++i) {
      histo_map["ROC " + std::to_string(roc_id) + " - ADC"]->Fill(
          i, subpacket.adc[i]);
    }
  }
  // if (event->GetDescription().compare("HCalTestBeamRaw") == 0) {
  //}
}
} // namespace eudaq
