#ifndef EUDAQ_DIPCLIENTPRODUCER_H
#define EUDAQ_DIPCLIENTPRODUCER_H

//---< eudaq >---//
#include "eudaq/Producer.hh"

//---< FiberTrackerDAQ > ---//
#include "FiberTrackerClient.h"

namespace eudaq {
class DipClientProducer : public eudaq::Producer {
public:
  /**
   * Constructor
   */
  DipClientProducer(const std::string &name,
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
  static const uint32_t factory_id_{eudaq::cstr2hash("DipClientProducer")};

private:

  std::shared_ptr<FiberTrackerClient> client;
  
  /// Output file path
  std::string output_path_{"."};

  /// File prefix
  std::string file_prefix_{"ldmx_test"};
};
} // namespace eudaq

#endif // EUDAQ_DIPCLIENTPRODUCER_H
