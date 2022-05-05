
#include "Reformat/RawDataFile.h"

#include <string>
#include <random>

namespace reformat::test {

/**
 * A dummy test file to check loading.
 */
class TestFile : public RawDataFile {
  int i_event_{0};
  int skip_{-1};
  static std::mt19937 rng;
  std::uniform_int_distribution<long int> time_jitter;
 public:
  /// test id to differentiate different test files
  static int id_;
  TestFile(const framework::config::Parameters& ps) 
    : RawDataFile(ps), time_jitter{-5,5} {
    num_ = ps.getParameter<int>("num");
    skip_ = ps.getParameter<int>("skip");
  }
  virtual std::optional<EventPacket> next() final override {
    i_event_++;
    // pretend that we dropped an event
    if (i_event_ == skip_) i_event_++;
    // leave early if "file" is done
    if (i_event_ > num_) return {};

    EventPacket ep;
    ep.setTimestamp(i_event_*10 + time_jitter(rng));
    ep.append(std::vector<uint8_t>(i_event_,22));
    return ep;
  }
 private:
  /// number of events to generate
  int num_;
};  // TestFile

// first id of test files
int TestFile::id_ = 1;
std::mt19937 TestFile::rng;

}

DECLARE_RAW_DATA_FILE(reformat::test::TestFile)
