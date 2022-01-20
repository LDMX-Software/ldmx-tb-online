#ifndef EUDAQ_HCALTESTBEAMPRODUCER_H
#define EUDAQ_HCALTESTBEAMPRODUCER_H

//---< ldmx-eudaq >---//
#include "eudaq/RogueTcpClientProducer.h"

namespace eudaq {
class HCalTestBeamProducer : public RogueTcpClientProducer {
public:
  /**
   * Constructor
   */
  HCalTestBeamProducer(const std::string &name,
                            const std::string &runcontrol);
  /**
   */
  void DoInitialise() override;

  /**
   */
  void DoConfigure() override;

  /// ID used to register this producer with the eudaq environment
  static const uint32_t factory_id_{
      eudaq::cstr2hash("HCalTestBeamProducer")};
};
} // namespace eudaq

#endif // EUDAQ_HCALTESTBEAMPRODUCER_H
