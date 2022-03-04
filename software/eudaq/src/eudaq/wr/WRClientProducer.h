#ifndef EUDAQ_WRCLIENTPRODUCER_H
#define EUDAQ_WRCLIENTPRODUCER_H

//---< eudaq >---//
#include "eudaq/Producer.hh"

//---< WR Timestamp > ---//
#include "WRClient.h"

namespace eudaq {
class WRClientProducer : public eudaq::Producer {
public:
  /**
   * Constructor
   */
  WRClientProducer(const std::string &name,
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
  static const uint32_t factory_id_{eudaq::cstr2hash("WRClientProducer")};

private:

  std::unique_ptr<WRClient> client{nullptr};
  
  /// Output file path
  std::string output_path_{"."};

  /// File prefix
  std::string file_prefix_{"ldmx_test"};
};
} // namespace eudaq

#endif // EUDAQ_WRCLIENTPRODUCER_H
