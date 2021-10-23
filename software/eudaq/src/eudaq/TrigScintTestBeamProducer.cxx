#include "eudaq/TrigScintTestBeamProducer.h"

//---< ldmx-eudaq >---//
#include "eudaq/TrigScintDataSender.h"

namespace {
auto dummy0 = eudaq::Factory<eudaq::Producer>::Register<
    eudaq::TrigScintTestBeamProducer, const std::string &, const std::string &>(
    eudaq::TrigScintTestBeamProducer::factory_id_);
}

namespace eudaq {

TrigScintTestBeamProducer::TrigScintTestBeamProducer(
    const std::string &name, const std::string &runcontrol)
    : RogueTcpClientProducer(name, runcontrol) {
  sender_ = TrigScintDataSender::create(this);
}

void TrigScintTestBeamProducer::DoInitialise() {
  /**
   * Add initialization here ...
   */

  // The base class init starts the rogue TCP client and initializes the file
  // writer.
  RogueTcpClientProducer::DoInitialise();
}

void TrigScintTestBeamProducer::DoConfigure() {

  /**
   * Add configuration here ...
   */

  // The base class config opens the rogue file writer.
  RogueTcpClientProducer::DoConfigure();
}

} // namespace eudaq
