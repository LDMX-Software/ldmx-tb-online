#include "eudaq/monitor/SimpleMonitor.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <ratio>
#include <thread>

namespace {
auto dummy0 = eudaq::Factory<eudaq::Monitor>::Register<
    eudaq::SimpleMonitor, const std::string &, const std::string &>(
    eudaq::SimpleMonitor::factory_id_);
}

namespace eudaq {

void SimpleMonitor::AtEventReception(EventSP ev) { 
	std::cout << "MONITOOOOOOORING" << std::endl;
	ev->Print(std::cout); 
}
} // namespace eudaq
