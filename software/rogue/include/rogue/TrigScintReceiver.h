#ifndef ROGUE_TSRECEIVER_H
#define ROGUE_TSRECEIVER_H

//---< C++ >---//
#include <string>

//---< rogue >---//
#include "rogue/interfaces/stream/Slave.h"

namespace eudaq {
class TrigScintReceiver : public rogue::interfaces::stream::Slave {
public:
  static std::shared_ptr<TrigScintReceiver> create() {
    static std::shared_ptr<TrigScintReceiver> ret =
        std::make_shared<TrigScintReceiver>();
    return (ret);
  }

  /// Constructor
  TrigScintReceiver() = default;
  
  /// Destructor
  ~TrigScintReceiver() = default;

  /**
   *
   */
  void acceptFrame(std::shared_ptr<rogue::interfaces::stream::Frame> frame);
  
  /**
   *
   */
  std::string convertToString(char *a, int size);
};
} // namespace eudaq

#endif
