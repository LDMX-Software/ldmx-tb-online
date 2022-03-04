#ifndef EUDAQ_TCPCOMMANDGENERATOR_H
#define EUDAQ_TCPCOMMANDGENERATOR_H

//---< StdLib >---//
#include <string>

//---< rogue >---//
#include "rogue/interfaces/stream/Frame.h"
#include "rogue/interfaces/stream/FrameIterator.h"
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
  void genFrame(const uint8_t &command);

  /**
   * Generate a data frame with a string command.
   *
   * @param command The string command to send.
   */
  virtual void genFrame(const std::string &command);

  /**
   *
   */
  template <class T> void genFrame(const std::string &varname, const T &value) {
    // Build the string command
    std::string command{"setVariable:"};
    command += varname + ":" + std::to_string(value);

    // Generate a frame
    genFrame(command);
  }

private:
  /**
   */
  template <class T> void send(T buffer, size_t size) {
    // Request a frame to copy the buffer into
    auto frame{reqFrame(size, true)};

    // Update frame payload size
    frame->setPayload(size);

    // Get an iterator to the beginning of the frame
    auto it{frame->begin()};

    // Copy the buffer into the frame
    toFrame(it, size, &buffer);

    // Send the frame to the TCP bridge for shipping
    sendFrame(frame);
  }
};
} // namespace eudaq
#endif // EUDAQ_TCPCOMMANDGENERATOR_H
