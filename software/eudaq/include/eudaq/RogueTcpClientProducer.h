#ifndef EUDAQ_ROGUETCPCLIENTPRODUCER_H
#define EUDAQ_ROGUETCPCLIENTPRODUCER_H

//---< eudaq >---//
#include "eudaq/Producer.hh"

//---< rogue >---//
#include "rogue/interfaces/stream/TcpClient.h"
#include "rogue/utilities/fileio/StreamWriter.h"
#include "rogue/utilities/fileio/StreamWriterChannel.h"
#include "rogue/TBStreamWriter.h"

//---< ldmx-eudaq >---//
#include "eudaq/RogueDataSender.h"
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

protected:
  /// Data Sender
  std::shared_ptr<RogueDataSender> sender_{nullptr};

  /// File writer
  //rogue::utilities::fileio::StreamWriterPtr writer_{rogue::utilities::fileio::StreamWriter::create()};

  rogue::TBStreamWriterPtr writer_{rogue::TBStreamWriter::create()};
  
  
  /// Output file path
  std::string output_path_{"."};
  
  /// File prefix
  std::string file_prefix_{"ldmx_test"};

private:
  /// TCP Bridge client
  rogue::interfaces::stream::TcpClientPtr tcp_;

  /// TCP command generator
  std::shared_ptr<TcpCommandGenerator> tcp_command_{
      TcpCommandGenerator::create()};

};
} // namespace eudaq

#endif // EUDAQ_ROGUETCPCLIENTPRODUCER_H
