#ifndef EUDAQ_TRIGSCINTTESTBEAMPRODUCER_H
#define EUDAQ_TRIGSCINTTESTBEAMPRODUCER_H

//---< ldmx-eudaq >---//
#include "eudaq/RogueTcpClientProducer.h"

namespace eudaq {
class TrigScintTestBeamProducer : public RogueTcpClientProducer {
public:
  /**
   * Constructor
   */
  TrigScintTestBeamProducer(const std::string &name,
                            const std::string &runcontrol);
  /**
   */
  void DoInitialise() override;

  /**
   */
  void DoConfigure() override;

  /// ID used to register this producer with the eudaq environment
  static const uint32_t factory_id_{
      eudaq::cstr2hash("TrigScintTestBeamProducer")};
};
} // namespace eudaq

#endif // EUDAQ_TRIGSCINTTESTBEAMPRODUCER_H
