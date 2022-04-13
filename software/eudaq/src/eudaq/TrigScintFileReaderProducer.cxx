
//---< C++ >---//
#include <bitset>
#include <chrono>
#include <sstream>
#include <string>
#include <fstream>

//---< eudaq >---//
#include "eudaq/RunControl.hh"
#include "eudaq/Producer.hh"

// us
#include "eudaq/FileReader.h"

class TrigScintFileReaderProducer : public eudaq::Producer {
 public:
  TrigScintFileReaderProducer(const std::string &name, const std::string &runcontrol);
  void DoInitialise() override;
  void DoConfigure() override;
  void DoStartRun() override;
  void DoStopRun() override;
  void DoTerminate() override;
  void DoReset() override;
  void RunLoop() override;
  static const uint32_t factory_id_{eudaq::cstr2hash("TrigScintFileReaderProducer")};
 private:
  /// Function to convert hex to binary
  uint64_t hex2Bin(const std::string& shex); 
 private:
  /// file to read from
  std::string filepath_;
  /// bytes to extract per event in file
  int n_bytes_per_event_;
 private:
  /// the file stream we are reading from
  FileReader file_;
};

namespace {
auto d = eudaq::Factory<eudaq::Producer>::Register<
    TrigScintFileReaderProducer, const std::string &, const std::string &>(
    TrigScintFileReaderProducer::factory_id_);
}

TrigScintFileReaderProducer::TrigScintFileReaderProducer(
    const std::string &name, const std::string &runcontrol)
    : eudaq::Producer(name, runcontrol) {}

void TrigScintFileReaderProducer::DoInitialise() {
  auto ini{GetInitConfiguration()};
}

void TrigScintFileReaderProducer::DoConfigure() {
  // Get the configuration
  auto conf{GetConfiguration()};

  // Get the file to open from the configuration
  filepath_ = conf->Get("FILE","");
  if (filepath_.empty()) {
    EUDAQ_THROW("No file given to TrigScientFileReader.");
  }
  EUDAQ_INFO("Reading events from " + filepath_);

  n_bytes_per_event_ = conf->Get("N_BYTES_PER_EVENT",1440);

  file_.open(filepath_);
  if (not file_) {
    EUDAQ_THROW("Unable to open file "+filepath_);
  }
}

void TrigScintFileReaderProducer::DoStartRun() {}

void TrigScintFileReaderProducer::DoStopRun() {
  file_.close();
}

void TrigScintFileReaderProducer::DoReset() {}

void TrigScintFileReaderProducer::DoTerminate() {}

void TrigScintFileReaderProducer::RunLoop() {
  auto event_count{0};
  
  while(file_) {
    std::vector<uint8_t> packet;
    if (file_.read(packet,n_bytes_per_event_)) {
      auto event{eudaq::Event::MakeUnique("TrigScintRaw")};
      event->AddBlock(0x2, packet);
      this->SendEvent(std::move(event));
    }
  }

  EUDAQ_WARN("End of file reached.");
  SetStatus(eudaq::Status::STATE_STOPPED, "Stopped");
}

