#include "eudaq/RogueTcpClientProducer.h"

//---< ldmx-rogue >---//
#include "rogue/Commands.h"

//---< ldmx-eudaq >---//
#include "eudaq/HCalDataSender.h"
#include "eudaq/TrigScintDataSender.h"

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

  // Configure the sender
  // TODO: Dynamically load these instead
  auto sender_type{ini->Get("SENDER", "HCal")}; 
  if (sender_type.compare("HCal") == 0) sender_ = HCalDataSender::create(this); 
  else if (sender_type.compare("TrigScint") == 0) sender_ = TrigScintDataSender::create(this); 
  else {
    EUDAQ_THROW("Failed to specify SENDER");
  }
  EUDAQ_INFO("Using a data sender of type " + sender_type); 

  // Connect the data sender to the TCP client
  tcp_->addSlave(sender_); 
  
  // Send the init command to the rogue server
  tcp_command_->genFrame(rogue::commands::init);
}

void RogueTcpClientProducer::DoConfigure() {

  // Get the configuration
  auto conf{GetConfiguration()};

  // Get the path to the output file
  output_path_ = conf->Get("OUTPUT_PATH", ".");

  // Get the file prefix
  file_prefix_ = conf->Get("ROGUE_FILE_PATTERN", "test"); 

  // Build the file name
  auto output_file{output_path_ + "/" + file_prefix_ + "_" + std::to_string(GetRunNumber()) + ".dat"}; 

  // First, make sure an existing file isn't open.
  if (writer_->isOpen()) writer_->close(); 

  // Open a file to write the stream
  writer_->open(output_file);

  EUDAQ_INFO("Writing rogue stream to " + output_file);
  
  // Send the config command to the rogue server
  tcp_command_->genFrame(rogue::commands::config);
}

void RogueTcpClientProducer::DoStartRun() {
  tcp_command_->genFrame(rogue::commands::start);
}

void RogueTcpClientProducer::DoStopRun() {
  tcp_command_->genFrame(rogue::commands::stop);

  // If open, close the file to which data is being written to.
  if (writer_->isOpen()) writer_->close(); 
}

void RogueTcpClientProducer::DoReset() {
  tcp_command_->genFrame(rogue::commands::reset);
}

void RogueTcpClientProducer::DoTerminate() {}

void RogueTcpClientProducer::RunLoop() {
}

} // namespace eudaq
