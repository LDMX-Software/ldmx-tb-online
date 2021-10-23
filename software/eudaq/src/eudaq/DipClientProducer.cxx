#include "eudaq/DipClientProducer.h"

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
  auto addr{ini->Get("TCP_ADDR", "127.0.0.1")};
  auto port{ini->Get("TCP_PORT", 8000)};

  std::string filenameBase="Test_DipClient";
  std::string outputPath="/u1/ldmx/data/dip_client/";
  std::string runNumber = std::to_string(GetRunNumber());
  //singleton pattern instead?
  client = std::make_shared<FiberTrackerClient>(runNumber,filenameBase,outputPath);
  
  EUDAQ_INFO("TCP client listening on " + addr + ":" + std::to_string(port));
  
}

void DipClientProducer::DoConfigure() {
  
  // Get the configuration
  auto conf{GetConfiguration()};

  // Get the path to the output file
  output_path_ = conf->Get("OUTPUT_PATH", ".");
  
  // Build the file name
  auto output_file{output_path_ + "/" + file_prefix_ + "_" + std::to_string(GetRunNumber()) + ".dat"}; 
  
}

void DipClientProducer::DoStartRun() {
  //Call the DIP client connect
  client->Subscribe();
  
}

void DipClientProducer::DoStopRun() {
  // Call the DIP client disconnect
  client->Unsubscribe();
    
}

void DipClientProducer::DoReset() {
  //Do something?
}

void DipClientProducer::DoTerminate() {}

void DipClientProducer::RunLoop() {
}

} // namespace eudaq
