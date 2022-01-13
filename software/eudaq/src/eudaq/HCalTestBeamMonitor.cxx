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
  // TODO Use DAQ map to fill map
  for (int i{256}; i < 264; ++i) {
    histo_map["ROC " + std::to_string(i) + " - ADC"] =
        m_monitor->Book<TH2D>("ROC " + std::to_string(i) + " ADC",
                              "ROC_" + std::to_string(i) + "_ADC", "",
                              ";Channel;ADC", 40, 0, 40, 200, 0, 200);
    m_monitor->SetDrawOptions(histo_map["ROC " + std::to_string(i) + " - ADC"],
                              "colz");
  }
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
      }
    }
  }
}
} // namespace eudaq
