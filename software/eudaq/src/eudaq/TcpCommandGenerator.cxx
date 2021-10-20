#include "eudaq/TcpCommandGenerator.h"

namespace eudaq {

void TcpCommandGenerator::genFrame(const uint8_t &command) {
  send<uint8_t>(command, 1);
}

void TcpCommandGenerator::genFrame(const std::string &command) {
  send<const char *>(command.data(), command.length());
}

} // namespace eudaq
