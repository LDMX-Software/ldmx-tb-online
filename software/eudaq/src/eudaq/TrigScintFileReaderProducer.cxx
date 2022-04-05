#include "eudaq/TrigScintFileReaderProducer.h" 

//---< C++ >---//
#include <bitset>
#include <chrono>
#include <sstream>
#include <string>

//---< eudaq >---//
#include "eudaq/RunControl.hh"

namespace {
auto dummy0 = eudaq::Factory<eudaq::Producer>::Register<
    eudaq::TrigScintFileReaderProducer, const std::string &, const std::string &>(
    eudaq::TrigScintFileReaderProducer::factory_id_);
}

namespace eudaq {

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
  auto file_path{conf->Get("FILE", "")};
  EUDAQ_INFO("Reading events from " + file_path);

  // Open the file for reading. If the file can't be opened, throw and
  // exception.
  ifile = std::make_shared<std::ifstream>(file_path.c_str(), std::ios::binary);
  if (!ifile->is_open()) {
    EUDAQ_THROW("Failed to open file " + file_path);
  }
}

void TrigScintFileReaderProducer::DoStartRun() {}

void TrigScintFileReaderProducer::DoStopRun() {
  // If open, close the file to which data is being written to.
  if (ifile->is_open())
    ifile->close();

  std::cout << "File closed: " << std::endl;
}

void TrigScintFileReaderProducer::DoReset() {}

void TrigScintFileReaderProducer::DoTerminate() {}

void TrigScintFileReaderProducer::RunLoop() {
  auto event_count{0};
  
  // The following reads a txt file with raw data line by line and extracts
  // events. Each event is encapsulated by the header/tail word 
  // 0xFFFFFFFFFFFFFFFF. Only words found within the header/tail are added 
  // to the event. Before being added to the event, the hex strings are 
  // converted to uint64_t.  

  // This is the vector used to hold an event.  This will be added to the
  // eudaq event and shipped to the data collector.
  std::vector<uint64_t> packet;

  // The following flag is set at the beginning of an event i.e. when the
  // value 0xFFFFFFFFFFFFFFFF is initially found. 
  bool event_found{false}; 
    
  for (std::string line; getline(*ifile.get(), line); ) {
    
    // Skip DAQ messages
    if (line[0] == 's' || line[0] == 'r' || line.empty()) { 
      continue; 
    }
    //std::cout << "[ TrigScintFileReaderProducer ]:  word: 0x" 
    //	      << line << std::endl; 
    // Convert the word from a string to a uint64_t
    uint64_t val{hex2Bin("0x"+line)}; 
    //std::cout << "[ TrigScintFileReaderProducer ]:  word: "
    //          << std::bitset<64>(val) << std::endl;

    if (!event_found && val != 0xFFFFFFFFFFFFFFFF) { 
      // Skip words until a begin of event tag is found.
      continue;
    } else if (event_found && val == 0xFFFFFFFFFFFFFFFF) { 
      // The end of an event has been reached.  Clear everything, ship the event
      // out and  and prepare for a new event.

      // Create an eudaq event to ship to the data collector.
      auto event{eudaq::Event::MakeUnique("TrigScintRaw")};
    
      // Copy the data block from the rogue frame
      event->AddBlock(0x2, packet);

      // Send the event
      SendEvent(std::move(event));

      packet.clear();
      event_found = false;

      // Increment event counter. This will be used in the future.
      ++event_count;

    } else if (!event_found && val == 0xFFFFFFFFFFFFFFFF) { 
      // A new event has been found, start adding words to the packet.
      event_found = true;
      continue;
    }
      packet.push_back(val); 
  }

  // Once all lines have been processed, the end of the file has been reached.
  // Stop the run at this point. 
  if (ifile->eof()) {
    EUDAQ_WARN("End of file reached.");
  }
}

uint64_t TrigScintFileReaderProducer::hex2Bin(const std::string& shex) { 
	std::stringstream ss;
  ss << std::hex << shex;
  uint64_t n;
  ss >> n; 
  return n; 
}

} // namespace eudaq
