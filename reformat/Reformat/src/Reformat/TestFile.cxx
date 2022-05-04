
#include "Reformat/RawDataFile.h"

#include <string>

namespace reformat::test {

/**
 * A dummy test file to check loading.
 */
class TestFile : public RawDataFile {
  int i_event_{0};
 public:
  /// test id to differentiate different test files
  static int id_;
  TestFile(const framework::config::Parameters& ps) : RawDataFile(ps) {
    num_ = ps.getParameter<int>("num");
    name_ = "TestBuffer"+std::to_string(id_++);
  }
  virtual std::string name() final override {
    return name_;
  } 
  virtual std::optional<EventPacket> next() final override {
    i_event_++;
    // leave early if "file" is done
    if (i_event_ > num_) return {};

    EventPacket ep;
    ep.timestamp = i_event_*10;
    ep.data = std::vector<uint8_t>(i_event_,22);
    return ep;
  }
 private:
  /// number of events to generate
  int num_;
  /// buffer name to output
  std::string name_;
};  // TestFile

// first id of test files
int TestFile::id_ = 1;

}

DECLARE_RAW_DATA_FILE(reformat::test::TestFile)
