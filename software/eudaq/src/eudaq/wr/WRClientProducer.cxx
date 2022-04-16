#include "WRClientProducer.h"

#include <string>

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

  EUDAQ_INFO("WR initialized");
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
  
  //Call the DIP client connect
  client->connect();

  exiting_run_ = 0;  
}

void WRClientProducer::DoStopRun() {
  // Call the DIP client disconnect
  client->disconnect();
  exiting_run_ = 1;
}

void WRClientProducer::DoReset() {
    client->terminate();
    exiting_run_ = 1;
}

void WRClientProducer::DoTerminate() {
    client->terminate();
    exiting_run_ = 1;
}

void WRClientProducer::RunLoop() {
    EUDAQ_INFO("Entered run loop");
    while (not exiting_run_) {
        std::vector<uint32_t> data_words = client->readAndClearOutputBuffer();

        if(data_words.size() > 0){
            const uint8_t *ptr = reinterpret_cast<const uint8_t*>(&data_words[0]);
            std::vector<uint8_t> data(ptr, ptr + sizeof(uint32_t)*data_words.size());
            auto size = data.size();
            auto frame = reqFrame(size, true);
            frame->setPayload(size);
            std::copy(data.begin(), data.end(), frame->begin());
            sendFrame(frame);
            auto ev = eudaq::Event::MakeUnique(event_name());
            SendEvent(std::move(ev));
        }

        std::this_thread::sleep_until(std::chrono::steady_clock::now()+std::chrono::milliseconds(500));
    }
}

} // namespace eudaq
