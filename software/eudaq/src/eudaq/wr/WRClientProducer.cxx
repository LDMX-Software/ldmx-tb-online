#include "WRClientProducer.h"

namespace {
auto dummy0 = eudaq::Factory<eudaq::Producer>::Register<
    eudaq::WRClientProducer, const std::string &, const std::string &>(
    eudaq::WRClientProducer::factory_id_);
}

namespace eudaq {

WRClientProducer::WRClientProducer(const std::string &name,
				     const std::string &runcontrol)
    : eudaq::Producer(name, runcontrol) {
} 

void WRClientProducer::DoInitialise() {
  auto ini{GetInitConfiguration()};
  
  // Get the TCP server address and server from the configuration
  // Default: localhost:8000

  EUDAQ_INFO("DIP Client initialized");
}

void WRClientProducer::DoConfigure() {
  
  // Get the configuration
  auto conf{GetConfiguration()};
  file_prefix_ = conf->Get("FILENAMEBASE", "WRClientData_");
  output_path_ = conf->Get("OUTPUTPATH","./");
  
  if (!client)
    client = std::make_unique<WRClient>();

  client->setFileName(file_prefix_);
  client->setOutputPath(output_path_);
}

void WRClientProducer::DoStartRun() {

  std::string runNumber = std::to_string(GetRunNumber());
  client->setRunNumber(runNumber);  
  
  client->connect();
  
}

void WRClientProducer::DoStopRun() {
  client->disconnect();
}

void WRClientProducer::DoReset() {
  
}

void WRClientProducer::DoTerminate() {
  client->terminate();
}

void WRClientProducer::RunLoop() {
}

} // namespace eudaq
