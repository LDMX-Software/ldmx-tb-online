
//---< C++ >---//
#include <bitset>
#include <chrono>
#include <fstream>

//---< eudaq >---//
#include "eudaq/RunControl.hh"
#include "eudaq/Producer.hh"

class Reader {
 public:
  /**
   * default constructor
   *
   * We make sure that our input file stream will not skip white space.
   */
  Reader() {
    file_.unsetf(std::ios::skipws);
  }

  /**
   * Open a file with this reader
   *
   * We open the file as an input, binary file.
   *
   * @param[in] file_name full path to the file we are going to open
   */
  void open(const std::string& file_name) {
    file_.open(file_name, std::ios::in | std::ios::binary);
    file_.seekg(0,std::ios::end);
    file_size_ = file_.tellg();
    file_.seekg(0);
  }

  /**
   * Close the file
   */
  void close() {
    file_.close();
  }

  /**
   * Constructor that also opens the input file
   * @see open
   * @param[in] file_name full path to the file we are going to open
   */
  Reader(const std::string& file_name) : Reader() {
    this->open(file_name);
  }

  /// destructor, close the input file stream
  ~Reader() = default;

  /**
   * Go ("seek") a specific position in the file.
   *
   * This non-template version of seek uses the default
   * meaning of the "off" parameter in which it counts bytes.
   *
   * @param[in] off number of bytes to move relative to dir
   * @param[in] dir location flag for the file, default is beginning
   */
  void seek(int off, std::ios_base::seekdir dir = std::ios::beg) {
    file_.seekg(off, dir);
  }

  /**
   * Seek by number of words
   *
   * This template version of seek uses the input word type
   * to move around by the count of the input words rather than
   * the count of bytes.
   *
   * @tparam[in] WordType Integral-type to count by
   * @param[in] off number of words to move relative to dir
   * @param[in] dir location flag for the file, default is beginning
   */
  template<typename WordType, std::enable_if_t<std::is_integral<WordType>::value,bool> = true>
  void seek(int off, std::ios_base::seekdir dir = std::ios::beg) {
    seek(off*sizeof(WordType), dir);
  }

  /**
   * Tell us where the reader is
   *
   * @return int number of bytes relative to beginning of file
   */
  int tell() { return file_.tellg(); }

  /**
   * Tell by number of words
   *
   * @return int number of words relative to beginning of file
   */
  template<typename WordType>
  int tell() { return tell()/sizeof(WordType); }

  /**
   * Read the next 'count' words into the input handle.
   *
   * This implementation of read is only available to pointers to integral types.
   * We assume that whatever space pointed to by w already has the space reserved
   * necessary for the input words.
   *
   * @tparam[in] WordType integral-type word to read out
   * @param[in] w pointer to WordType array to write data to
   * @param[in] count number of words to read
   * @return (*this)
   */
  template <typename WordType, std::enable_if_t<std::is_integral<WordType>::value,bool> = true>
  Reader& read(WordType* w, std::size_t count) {
    file_.read(reinterpret_cast<char*>(w), sizeof(WordType)*count);
    return *this;
  }

  /**
   * Stream the next word into the input handle
   *
   * This implementation of the stream operator is only available to handles of integral types.
   * Helps for shorthand of only grabbing a single word from the reader.
   *
   * @see read
   *
   * @tparam[in] WordType integral-type word to read out
   * @param[in] w reference to word to read into
   * @return handle to modified reader
   */
  template <typename WordType, std::enable_if_t<std::is_integral<WordType>::value,bool> = true>
  Reader& operator>>(WordType& w) {
    return read(&w, 1);
  }

  /**
   * Read through the iterator objects
   */
  template <typename It>
  Reader& read(It beg,It end) {
    for (auto it{beg}; it != end; it = std::next(it)) {
      if (!(*this >> *it)) return *this;
    }
    return *this;
  }

  /**
   * Read the next 'count' objects into the input vector.
   *  
   * This is common enough, I wanted to specialize the read function.
   *
   * The std::vector::resize is helpful for avoiding additional 
   * copies while the vector is being expanded. After allocating the space
   * for each entry in the vector, we call the stream operator from
   * *this into each entry in order, leaving early if a failure occurs.
   *
   * @tparam[in] ContentType type of object inside the vector
   * @param[in] vec object vector to read into
   * @param[in] count number of objects to read
   * @return *this
   */
  template <typename ContentType>
  Reader& read(std::vector<ContentType>& vec, std::size_t count) {
    vec.resize(count);
    return read(vec.begin(),vec.end());
  }

  /**
   * Check if reader is in a fail state
   *
   * Following the C++ reference, we pass-along the check
   * on if our ifstream is in a fail state.
   *
   * @return bool true if ifstream is in fail state
   */
  bool operator!() const {
    return file_.fail();
  }

  /**
   * Check if reader is in good/bad state
   *
   * Following the C++ reference, we pass-along the check
   * on if our ifstream is in a fail state.
   *
   * Defining this operator allows us to do the following.
   *
   * Reader r('dummy.raw')
   * if (r) {
   *   std::cout << "dummy.raw was opened good" << std::endl;
   * }
   *
   * @return bool true if ifstream is in good state
   */
  operator bool() const {
    return !file_.fail();
  }

  /**
   * check if file is done
   *
   * Just calls the underlying ifstream eof.
   *
   * @return true if we have reached the end of file.
   */
  bool eof() {
    return file_.eof() or file_.tellg() == file_size_;
  }

 private:
  /// file stream we are reading from
  std::ifstream file_;
  /// file size in bytes
  std::size_t file_size_;
};  // RawDataFile

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
  Reader file_;
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

    // Send the event
    SendEvent(std::move(event));

    // Increment event counter. This will be used in the future.
    ++event_count;
  }

  EUDAQ_WARN("End of file reached.");
  SetStatus(eudaq::Status::STATE_STOPPED, "Stopped");
}
