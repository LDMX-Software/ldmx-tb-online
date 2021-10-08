#ifndef EUDAQ_TCPCOMMANDGENERATOR_H
#define EUDAQ_TCPCOMMANDGENERATOR_H

//---< StdLib >---//
#include <string>

//---< rogue >---//
#include "rogue/interfaces/stream/Master.h"

namespace eudaq {
class TcpCommandGenerator : public rogue::interfaces::stream::Master {
public:
  static std::shared_ptr<TcpCommandGenerator> create() {
    static std::shared_ptr<TcpCommandGenerator> ret =
        std::make_shared<TcpCommandGenerator>();
    return (ret);
  }

  /// Constructor
  TcpCommandGenerator() = default; 

  /// Default Constructor
  ~TcpCommandGenerator() = default;
  
  /**
   * Generate a data frame with a string command.
   *
   * @param command The encoded command to send.
   */
  virtual void genFrame(uint8_t command);
};
} // namespace eudaq
#endif // EUDAQ_TCPCOMMANDGENERATOR_H
