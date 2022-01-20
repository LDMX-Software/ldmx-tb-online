#include "eudaq/TestBeamDataCollector.h"

#include <iostream>

namespace {
auto dummy0 = eudaq::Factory<eudaq::DataCollector>::Register<
    eudaq::TestBeamDataCollector, const std::string &, const std::string &>(
    eudaq::TestBeamDataCollector::factory_id_);
}

namespace eudaq {
void TestBeamDataCollector::DoConfigure() {
  /*m_noprint = 0;
  auto conf = GetConfiguration();
  if (conf) {
    conf->Print();
    m_noprint = conf->Get("DISABLE_PRINT", 0);
  }*/
}

void TestBeamDataCollector::DoReceive(ConnectionSPC id, EventSP ev) {
  // if (!m_noprint)
	std::cout << "I'm here" << std::endl;
  ev->Print(std::cout);

  WriteEvent(ev);
}
} // namespace eudaq
