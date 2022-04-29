
//---< C++ >---//
#include <bitset>
#include <chrono>

//---< eudaq >---//
#include "eudaq/RunControl.hh"
#include "eudaq/Producer.hh"

#include "eudaq/FileReader.h"

// uncomment to test raw decoder during loop
//#include "eudaq/HgcrocRawDecode.h"

struct hex {
  uint32_t& w_;
  hex(uint32_t& w) : w_{w} {}
};

std::ostream& operator<<(std::ostream& os, const hex& h) {
  os << "0x" << std::setfill('0') << std::setw(8) << std::hex
     << h.w_ << std::dec << std::setfill(' ');
  return os;
}

class HgcrocFileReaderProducer : public eudaq::Producer {
 public:
  HgcrocFileReaderProducer(const std::string &name, const std::string &runcontrol);
  void DoInitialise() override;
  void DoConfigure() override;
  void DoStartRun() override;
  void DoStopRun() override;
  void DoTerminate() override;
  void DoReset() override;
  void RunLoop() override;
  static const uint32_t factory_id_{eudaq::cstr2hash("HgcrocFileReaderProducer")};
 private:
  FileReader file_;
};

namespace {
auto dummy0 = eudaq::Factory<eudaq::Producer>::Register<
    HgcrocFileReaderProducer, const std::string &, const std::string &>(
    HgcrocFileReaderProducer::factory_id_);
}

HgcrocFileReaderProducer::HgcrocFileReaderProducer(
    const std::string &name, const std::string &runcontrol)
    : eudaq::Producer(name, runcontrol) {}

void HgcrocFileReaderProducer::DoInitialise() {
  auto ini{GetInitConfiguration()};
}

void HgcrocFileReaderProducer::DoConfigure() {
  // Get the configuration
  auto conf{GetConfiguration()};

  // Get the file to open from the configuration
  auto file_path{conf->Get("FILE", "")};
  EUDAQ_INFO("Reading events from " + file_path);

  // Open the file for reading. If the file can't be opened, throw and
  // exception.
  file_.open(file_path);
  if (!file_) {
    EUDAQ_THROW("Failed to open file " + file_path);
  }
}

void HgcrocFileReaderProducer::DoStartRun() {}

void HgcrocFileReaderProducer::DoStopRun() {
  file_.close();
}

void HgcrocFileReaderProducer::DoReset() {}

void HgcrocFileReaderProducer::DoTerminate() {}

void HgcrocFileReaderProducer::RunLoop() {
  auto event_count{0};
  uint32_t w; // dummy word for reading
  while (file_) {
    do {
      file_ >> w;
    } while(file_ and w != 0xbeef2021 and w != 0xbeef2022);
    // catch trailing words or events only half read out trailing words
    if (!file_) break;

    // w is the signal word now.
    std::vector<uint32_t> packet;
    packet.push_back(w);

    // event header
    file_ >> w;
    packet.push_back(w);
    uint32_t version  = (w >> 28) & 0xf;
    uint32_t eventlen = w & 0xfff;
    if (version == 1u) {
      // eventlen is 32-bit words in event
      // do nothing here
    } else if (version == 2u) {
      // eventlen is 64-bit words in event,
      // need to multiply by 2 to get actual 32-bit event length
      eventlen *= 2;
      // and subtract off the special header word above
      eventlen -= 1;
    } else {
      EUDAQ_THROW("HgcrocFileReaderProducer can only decode event length for versions 1 and 2 of DAQ format.");
    }

    // total packet size is eventlen plus the special header word (the beefs)
    packet.resize(eventlen+1);
    // read from the 3rd word until end of event
    file_.read(packet.begin()+2,packet.end());

    // Create an eudaq event to ship to the data collector.
    auto event{eudaq::Event::MakeUnique("HGCROCRaw")};

    // Copy the data block from the rogue frame
    event->AddBlock(0x1, packet);

    /**
     * helpful for development of decoding test
    EUDAQ_DEBUG("Get block back from event");
    auto bl = event->GetBlock(1);
    EUDAQ_DEBUG("Test decoding of packet...");
    hcal::decode(bl);
    EUDAQ_DEBUG("Done test decoding.");
    */

    // Send the event
    SendEvent(std::move(event));

    // Increment event counter. This will be used in the future.
    ++event_count;
  }

  EUDAQ_WARN("End of file reached.");
  SetStatus(eudaq::Status::STATE_STOPPED, "Stopped");
}
