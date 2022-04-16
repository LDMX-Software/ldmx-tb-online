#ifndef EUDAQ_WRCLIENTPRODUCER_H
#define EUDAQ_WRCLIENTPRODUCER_H

//---< eudaq >---//
#include "eudaq/Producer.hh"
#include "eudaq/RogueDataSender.h"

//---< rogue >---//
#include <rogue/utilities/fileio/StreamWriter.h>
#include <rogue/utilities/fileio/StreamWriterChannel.h>
#include <rogue/interfaces/stream/Master.h>
#include <rogue/interfaces/stream/Slave.h>
#include <rogue/interfaces/stream/TcpClient.h>
#include <rogue/interfaces/stream/Frame.h>
#include <rogue/interfaces/stream/FrameIterator.h>
#include <rogue/interfaces/stream/FrameLock.h>

//---< WR Timestamp > ---//
#include "WRClient.h"

namespace eudaq {
class WRClientProducer : public eudaq::Producer, public rogue::interfaces::stream::Master {
public:
  std::string event_name() const {
    return "WR";
  }

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

  int exiting_run_ = 0; 

};
} // namespace eudaq

#endif // EUDAQ_WRCLIENTPRODUCER_H
