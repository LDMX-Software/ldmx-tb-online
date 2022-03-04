#include "DipClientProducer.h"

namespace {
auto dummy0 = eudaq::Factory<eudaq::Producer>::Register<
    eudaq::DipClientProducer, const std::string &, const std::string &>(
    eudaq::DipClientProducer::factory_id_);
}

namespace eudaq {

DipClientProducer::DipClientProducer(const std::string &name,
				     const std::string &runcontrol)
    : eudaq::Producer(name, runcontrol) {
} 

void DipClientProducer::DoInitialise() {
  auto ini{GetInitConfiguration()};
  
  // Get the TCP server address and server from the configuration
  // Default: localhost:8000

  EUDAQ_INFO("DIP Client initialized");


  
}

void DipClientProducer::DoConfigure() {
  
  // Get the configuration
  auto conf{GetConfiguration()};
  file_prefix_ = conf->Get("FILENAMEBASE", "dipClientData_");
  output_path_ = conf->Get("OUTPUTPATH","./");

  if (!client)
    client = std::make_unique<FiberTrackerClient>("",file_prefix_,output_path_);
  else {
    client->setFileNameBase(file_prefix_);
    client->setOutputPath(output_path_);
  }
}

void DipClientProducer::DoStartRun() {

  std::string runNumber = std::to_string(GetRunNumber());
  client->setRunNumber(runNumber);  
  
  //Call the DIP client connect
  client->Subscribe();
  
}

void DipClientProducer::DoStopRun() {
  // Call the DIP client disconnect
  client->Unsubscribe();
}

void DipClientProducer::DoReset() {
  
}

void DipClientProducer::DoTerminate() {
  client.reset(nullptr);
  
}

void DipClientProducer::RunLoop() {
}

} // namespace eudaq
