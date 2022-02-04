#ifndef EUDAQ_CAPTANTRIGSCINTTESTBEAMPRODUCER_H
#define EUDAQ_CAPTANTRIGSCINTTESTBEAMPRODUCER_H

//---< ldmx-eudaq >---//
#include "eudaq/RogueTcpClientProducer.h"

//---< rogue >---//
#include "rogue/TsUdpClient.h"

namespace eudaq {
  class CaptanTrigScintTestBeamProducer : public RogueTcpClientProducer {

  private:

    std::shared_ptr<TsUdpClient> udp_client_;

    
  public:
    /**
     * Constructor
     */
    CaptanTrigScintTestBeamProducer(const std::string &name,
				    const std::string &runcontrol);
    /**
     */
    void DoInitialise() override;

    /**
     */
    void DoConfigure() override;
    
    
    void DoStartRun() override;
    
    
    void DoStopRun() override;
    
    /// ID used to register this producer with the eudaq environment
    static const uint32_t factory_id_{
      eudaq::cstr2hash("CaptanTrigScintTestBeamProducer")};
  };
} // namespace eudaq

#endif // EUDAQ_CAPTANTRIGSCINTTESTBEAMPRODUCER_H
