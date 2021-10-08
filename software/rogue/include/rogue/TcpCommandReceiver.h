#ifndef ROGUE_TCPCOMMANDRECEIVER_H
#define ROGUE_TCPCOMMANDRECEIVER_H

//---< rogue >---//
#include "rogue/interfaces/stream/Slave.h"

namespace rogue {
class TcpCommandReceiver : public rogue::interfaces::stream::Slave {
public:
  static std::shared_ptr<TcpCommandReceiver> create() {
    static std::shared_ptr<TcpCommandReceiver> ret =
        std::make_shared<TcpCommandReceiver>();
    return (ret);
  }

  /// Constructor 
  TcpCommandReceiver() = default;

  /// Destructor
  ~TcpCommandReceiver() = default;

  /**
   */
  void
  acceptFrame(std::shared_ptr<rogue::interfaces::stream::Frame> frame);

  /**
   *
   */
  bool startRun() { return start_run_; }

  bool stopRun() { return stop_run_; } 

private: 
  /// Flag indicating whether a run should be started.
  bool start_run_{false};

  /// Flag indicating whether a run should be stopped.
  bool stop_run_{false}; 
};
} // namespace rogue
#endif // ROGUE_TCPCOMMANDRECEIVERS_H
