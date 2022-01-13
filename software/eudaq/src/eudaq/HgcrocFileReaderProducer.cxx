#include "eudaq/HgcrocFileReaderProducer.h"

//---< C++ >---//
#include <bitset>
#include <chrono>

//---< eudaq >---//
#include "eudaq/RunControl.hh"

namespace {
auto dummy0 = eudaq::Factory<eudaq::Producer>::Register<
    eudaq::HgcrocFileReaderProducer, const std::string &, const std::string &>(
    eudaq::HgcrocFileReaderProducer::factory_id_);
}

namespace eudaq {

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
  ifile = std::make_shared<std::ifstream>(file_path.c_str(), std::ios::binary);
  if (!ifile->is_open()) {
    EUDAQ_THROW("Failed to open file " + file_path);
  }
}

void HgcrocFileReaderProducer::DoStartRun() {}

void HgcrocFileReaderProducer::DoStopRun() {
  // If open, close the file to which data is being written to.
  if (ifile->is_open())
    ifile->close();
}

void HgcrocFileReaderProducer::DoReset() {}

void HgcrocFileReaderProducer::DoTerminate() {}

void HgcrocFileReaderProducer::RunLoop() {
  auto event_count{0};
  while (true) {

    // Extract the header. The first two words are fixed to the following
    // 00:31: 0x11888811
    // 32:63: 0xBEEF2021
    std::vector<uint32_t> packet(3, 0x0);
    ifile->read(reinterpret_cast<char *>(&packet[0]), sizeof(uint32_t));

    // Given that the data blocks are being extracted in fixed
    // size blocks, the only time an EOF can occur is after the
    // initial read.
    if (ifile->eof()) {
      EUDAQ_WARN("End of file reached.");
      SetStatus(Status::STATE_STOPPED, "Stopped");
      break;
    }
    ifile->read(reinterpret_cast<char *>(&packet[1]), sizeof(uint32_t));
    ifile->read(reinterpret_cast<char *>(&packet[2]), sizeof(uint32_t));

    // The 3rd word in the header contains the total number of 32-bit words
    // in the packet
    auto size{packet[2] & 0xFFF};

    // Now that the size is known, form the packet that will be sent to
    // the data collector. Note, the size does not include the two initial
    // header and two tail words. Since the first 3 words have already been
    // copied, then we only need to copy (size - 1 header word + 2 tail words)
    packet.resize(size + 4, 0x0);
    ifile->read(reinterpret_cast<char *>(&packet[3]),
                sizeof(uint32_t) * (size + 1));
    //std::cout << "[ HgcrocFileReaderProducer ]:  word ( 0 ) : "
    //          << std::bitset<32>(packet[3]) << std::endl;
    //std::cout << "[ HgcrocFileReaderProducer ]:  word ( 1 ) : "
    //          << std::bitset<32>(packet[4]) << std::endl;

    // Create an eudaq event to ship to the data collector.
    auto event{eudaq::Event::MakeUnique("HGCROCRaw")};

    // Copy the data block from the rogue frame
    event->AddBlock(0x1, packet);

    // Send the event
    SendEvent(std::move(event));

    // Increment event counter. This will be used in the future.
    ++event_count;
  }
}

} // namespace eudaq
