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
   * Generate a data frame with a uint command.
   *
   * @param command The encoded command to send.
   */
  virtual void genFrame(uint8_t command);

  /** Generate a data frame with a string command.
   *
   *
   * @param command The string command to send.
   */
  virtual void send_cmd(const std::string& command);

  virtual void send_float(const float& value);
  virtual void send_buffer(char buffer[], const uint& size);
  virtual void send_var(const std::string& varname, const float& value);
  
  
};
} // namespace eudaq
#endif // EUDAQ_TCPCOMMANDGENERATOR_H
