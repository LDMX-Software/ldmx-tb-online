#include "eudaq/RogueTcpClientProducer.h"

//---< ldmx-rogue >---//
#include "rogue/Commands.h"

namespace {
auto dummy0 = eudaq::Factory<eudaq::Producer>::Register<
    eudaq::RogueTcpClientProducer, const std::string &, const std::string &>(
    eudaq::RogueTcpClientProducer::factory_id_);
}

namespace eudaq {

RogueTcpClientProducer::RogueTcpClientProducer(const std::string &name,
                                               const std::string &runcontrol)
    : eudaq::Producer(name, runcontrol) {

} 

void RogueTcpClientProducer::DoInitialise() {
  auto ini{GetInitConfiguration()};

  // Get the TCP server address and server from the configuration
  // Default: localhost:8000
  auto addr{ini->Get("TCP_ADDR", "127.0.0.1")};
  auto port{ini->Get("TCP_PORT", 8000)};
  EUDAQ_INFO("TCP client listening on " + addr + ":" + std::to_string(port));

  // Open the TCP client
  tcp_ = rogue::interfaces::stream::TcpClient::create(addr, port);

  // Connect the tcp client to the tcp command generator
  tcp_command_->addSlave(tcp_);

  // Set how much data to buffer before writing
  writer_->setBufferSize(10000);

  // Connect the tcp stream to the file writer
  tcp_->addSlave(writer_->getChannel(0)); 

  // Get the path to the output file
  output_path_ = ini->Get("OUTPUT_PATH", ".");

  // Get the file prefix
  file_prefix_ = ini->Get("FILE_PREFIX", "ldmx_test"); 

}

void RogueTcpClientProducer::DoConfigure() {
  
  // First, make sure an existing file isn't open.
  if (writer_->isOpen()) writer_->close(); 

  // Open a file to write the stream
  // TODO: Append the run number to this?
  writer_->open(output_path_ + "/" + file_prefix_ + ".dat"); 

}

void RogueTcpClientProducer::DoStartRun() {
  tcp_command_->genFrame(rogue::commands::start);
}

void RogueTcpClientProducer::DoStopRun() {
  tcp_command_->genFrame(rogue::commands::stop);

  // If open, close the file to which data is being written to.
  if (writer_->isOpen()) writer_->close(); 
}

void RogueTcpClientProducer::DoReset() {}

void RogueTcpClientProducer::DoTerminate() {}

void RogueTcpClientProducer::RunLoop() {
  auto event{eudaq::Event::MakeUnique("LDMXTestBeamRaw")};
}

} // namespace eudaq
