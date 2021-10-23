#include "eudaq/HCalTestBeamProducer.h"

//---< ldmx-eudaq >---//
#include "eudaq/HCalDataSender.h"

namespace {
auto dummy0 = eudaq::Factory<eudaq::Producer>::Register<
    eudaq::HCalTestBeamProducer, const std::string &, const std::string &>(
    eudaq::HCalTestBeamProducer::factory_id_);
}

namespace eudaq {

HCalTestBeamProducer::HCalTestBeamProducer(
    const std::string &name, const std::string &runcontrol)
    : RogueTcpClientProducer(name, runcontrol) {
  sender_ = HCalDataSender::create(this);
}

void HCalTestBeamProducer::DoInitialise() {
  /**
   * Add initialization here ...
   */

  // The base class init starts the rogue TCP client and initializes the file
  // writer.
  RogueTcpClientProducer::DoInitialise();
}

void HCalTestBeamProducer::DoConfigure() {

  /**
   * Add configuration here ...
   */

  // The base class config opens the rogue file writer.
  RogueTcpClientProducer::DoConfigure();
}

} // namespace eudaq
