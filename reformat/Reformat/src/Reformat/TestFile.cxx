
#include "Reformat/RawDataFile.h"

#include <string>

namespace reformat {
namespace test {

/**
 * A dummy test file to check loading.
 */
class TestFile : public RawDataFile {
 public:
  /// test id to differentiate different test files
  static int id_;
  TestFile(const framework::config::Parameters& ps) : RawDataFile(ps) {
    num_ = ps.getParameter<int>("num");
    name_ = "TestBuffer"+std::to_string(id_++);
  }
  bool next(framework::Event& event) {
    if (event.getEventNumber() <= num_) {
      std::vector<uint64_t> test_buffer(event.getEventNumber(),22);
      event.add(name_,test_buffer);
      return true;
    } else {
      return false;
    }
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
}

DECLARE_RAW_DATA_FILE(reformat::test,TestFile)
