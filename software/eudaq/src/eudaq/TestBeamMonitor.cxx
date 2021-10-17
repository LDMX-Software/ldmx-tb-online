#include "eudaq/TestBeamMonitor.h"

#include <iostream>

#include "TH1.h"

namespace {
auto dummy0 = eudaq::Factory<eudaq::Monitor>::Register<
    eudaq::TestBeamMonitor, const std::string &, const std::string &>(
    eudaq::TestBeamMonitor::factory_id_);
}

namespace eudaq {

void TestBeamMonitor::AtConfiguration() { 
  // TODO Use DAQ map to fill map
  histo_map["roc 1, channel 1"] = m_monitor->Book<TH1D>("roc 1, channel 1", "",
    "", "", 100, 0., 1000.);

}

void TestBeamMonitor::AtEventReception(EventSP event) { 
	std::cout << event->GetDeviceN() << std::endl; 
  // Fill HCal plots
  // TODO Create and event type to simplify extracting of the data
  if (event->GetDescription().compare("HCalTestBeamRaw") == 0) {
    std::vector<uint8_t> data{event->GetBlock(0x1)};
    auto size{data.size() - 8 - 4};
    auto roc_subpacket_size{42*4};

    auto start{8 + size%roc_subpacket_size + 2};
    
    auto roc_id{data[start + 1] << 8 | data[start]}; 

    start += 14;
    auto tdc{getField(data[start + 1] << 8 | data[start], 9, 0)};
    
    histo_map["roc 1, channel 1"]->Fill(tdc); 

  } else if (event->GetDeviceN() == 0x2) {
    std::cout << "Making Trigger Scint Plots" << std::endl;
  }

	//event->Print(std::cout); 
}
} // namespace eudaq
