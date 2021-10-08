#ifndef EUDAQ_ROGUETCPCLIENTPRODUCER_H
#define EUDAQ_ROGUETCPCLIENTPRODUCER_H

//---< eudaq >---//
#include "eudaq/Producer.hh"

//---< rogue >---//
#include "rogue/interfaces/stream/TcpClient.h"

//---< ldmx-eudaq >---//
#include "eudaq/TcpCommandGenerator.h"

namespace eudaq {
class RogueTcpClientProducer : public eudaq::Producer {
public:
  /**
   * Constructor
   */
  RogueTcpClientProducer(const std::string &name,
                         const std::string &runcontrol);
  /**
   */
  void DoInitialise() override;

  /**
   */
  void DoConfigure() override;

  /**
   */
  void DoStartRun() override;

  /**
   */
  void DoStopRun() override;

  /**
   */
  void DoTerminate() override;

  /**
   */
  void DoReset() override;

  /**
   */
  void RunLoop() override;

  /// ID used to register this producer with the eudaq environment
  static const uint32_t factory_id_{eudaq::cstr2hash("RogueTcpClientProducer")};

private:
  /// TCP Bridge client
  rogue::interfaces::stream::TcpClientPtr tcp_;

  /// TCP command generator
  std::shared_ptr<TcpCommandGenerator> tcp_command_{
      TcpCommandGenerator::create()};
};
} // namespace eudaq

#endif // EUDAQ_ROGUETCPCLIENTPRODUCER_H
