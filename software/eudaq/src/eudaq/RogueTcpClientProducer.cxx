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
} //, m_file_lock(0), m_exit_of_run(false) {}

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
}

void RogueTcpClientProducer::DoConfigure() {}

void RogueTcpClientProducer::DoStartRun() {
  tcp_command_->genFrame(rogue::commands::start);
}

void RogueTcpClientProducer::DoStopRun() {
  tcp_command_->genFrame(rogue::commands::stop);
}

void RogueTcpClientProducer::DoReset() {}

void RogueTcpClientProducer::DoTerminate() {}

void RogueTcpClientProducer::RunLoop() {}

} // namespace eudaq
