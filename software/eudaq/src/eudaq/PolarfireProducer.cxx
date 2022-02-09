
#include <iostream>
#include <memory>

#include <rogue/utilities/fileio/StreamWriter.h>
#include <rogue/utilities/fileio/StreamWriterChannel.h>
#include <rogue/interfaces/stream/Master.h>
#include <rogue/interfaces/stream/Frame.h>
#include <rogue/interfaces/stream/FrameIterator.h>

#include <eudaq/Producer.hh>

#include "pflib/PolarfireTarget.h"
#include "pflib/rogue/RogueWishboneInterface.h"

/**
 * Interface to a single polarfire 
 */
class PolarfireProducer : public eudaq::Producer, public rogue::interfaces::stream::Master {
 public:
  PolarfireProducer(const std::string & name, const std::string & runcontrol);
  void DoInitialise() override;
  void DoConfigure() override;
  void DoStartRun() override;
  void DoStopRun() override;
  void DoTerminate() override;
  void DoReset() override;
  void RunLoop() override;
  
  static const uint32_t factory_id_ = eudaq::cstr2hash("PolarfireProducer");
 private:
  /// connection to polarfire
  std::unique_ptr<pflib::PolarfireTarget> pft_;
  /// how daq event reading is triggered
  enum class L1A_MODE {
    PEDESTAL,
    CHARGE,
    EXTERNAL
  } the_l1a_mode_;
  /// ID for fpga connected to this polarfire
  int fpga_id_;
  /// file writer
  rogue::utilities::fileio::StreamWriterPtr writer_{
    rogue::utilities::fileio::StreamWriter::create()};
  /// output file path
  std::string output_path_;
  /// file prefix (suffix is number generated by file writer)
  std::string file_prefix_{"ldmx_hcal"};
};

/**
 * This code registers our class with the central eudaq factory
 * so that it can be constructed when needed
 */
namespace {
auto d = eudaq::Factory<eudaq::Producer>::Register<
    PolarfireProducer, const std::string&, const std::string&>(
    PolarfireProducer::factory_id_);
}

PolarfireProducer::PolarfireProducer(const std::string & name, const std::string & runcontrol)
  : eudaq::Producer(name, runcontrol),
    rogue::interfaces::stream::Master() {}

/**
 * Initialization procedures
 *
 * We open the connection to the wishbone interface and pass
 * this interface connection to our Hcal object which holds 
 * more of the "high-level" functions. We also open the connection
 * to the backend. With Rogue, the wishbone interface and
 * the backend are the same.
 *
 * This is where we decide what type of readout mode will be done
 *  - DMA/stream (not implemented or tested yet)
 *  - MemMap (currently only one supported)
 */
void PolarfireProducer::DoInitialise(){
  auto ini = GetInitConfiguration();

  auto addr{ini->Get("TCP_ADDR", "127.0.0.1")};
  auto port{ini->Get("TCP_PORT", 8000)};
  EUDAQ_INFO("TCP client listening on " + addr + ":" + std::to_string(port));

  auto ro_mode{ini->Get("RO_MODE", "MemMap")};
  EUDAQ_INFO("Readout Mode set to " + ro_mode);

  if (ro_mode == "MemMap") {
    auto ptr{new pflib::rogue::RogueWishboneInterface(addr,port)};
    pft_ = std::make_unique<pflib::PolarfireTarget>(ptr,ptr);
  } else {
    EUDAQ_THROW("Unrecognized readout mode "+ro_mode);
  }

  auto l1a_mode{ini->Get("L1A_MODE","PEDESTAL")};
  EUDAQ_INFO("L1A Trigger Mode set to " + l1a_mode);
  if (l1a_mode == "PEDESTAL") {
    the_l1a_mode_ = L1A_MODE::PEDESTAL;
  } else if (l1a_mode == "CHARGE") {
    the_l1a_mode_ = L1A_MODE::CHARGE;
  } else if (l1a_mode == "EXTERNAL") {
    the_l1a_mode_ = L1A_MODE::EXTERNAL;
  } else {
    EUDAQ_THROW("Unrecognized L1A mode "+l1a_mode);
  }

  // how much data to buffer before writing
  writer_->setBufferSize(ini->Get("FILE_BUFF_SIZE",10000));
}

/**
 * Configuration of connection to polarfire
 *
 * Cases to Handle:
 *  - instance of this class for each polarfire in real world
 *  - specify which polarfire we are
 *  - specify which ROCs are actually there
 *  - every ROC has its own config file
 *  - beam runs, charge injection runs, ...
 *  - e.g. set charge injection amplitude from here
 *  - (pflib) get links up and running properly
 *  - stream vs memory map readout mode from rogue POV
 */
void PolarfireProducer::DoConfigure(){
  auto conf = GetConfiguration();

  // output file writing configuration
  output_path_ = conf->Get("OUTPUT_PATH",".");
  file_prefix_ = conf->Get("FILE_PREFIX","ldmx_hcal");
  if (writer_->isOpen()) writer_->close();
  std::stringstream output_file;
  output_file << output_path_ << "/" << file_prefix_ << "_run_" << GetRunNumber() << ".raw";
  EUDAQ_INFO("Writing data stream to "+output_file.str());
  writer_->open(output_file.str());
  this->addSlave(writer_->getChannel(0));

  // or in independent config file?
  fpga_id_ = conf->Get("FPGA_ID", 0);

  auto& daq = pft_->hcal->daq();
}
/**
 * Start of run, softer reset to start cleanly
 *  - Jeremy's branch
 *  - flags for "external" or various "local" modes
 */
void PolarfireProducer::DoStartRun(){
  pft_->prepareNewRun();
}

/**
 * Clean close
 */
void PolarfireProducer::DoStopRun(){
  if (writer_->isOpen()) writer_->close();
}
/**
 * typically used in failure modes, check out state function
 * i.e. recover from failure
 */
void PolarfireProducer::DoReset(){
}
/**
 * Not sure what this does...
 */
void PolarfireProducer::DoTerminate(){
}
/**
 * Include some "mode" about if sending our own L1A ("local" mode)
 * or some "external" mode where L1A is generated elsewhere
 */
void PolarfireProducer::RunLoop(){
  // depending on configured mode, we trigger daq readout or not
  switch(the_l1a_mode_) {
    case L1A_MODE::PEDESTAL:
      pft_->backend->fc_sendL1A();
      break;
    case L1A_MODE::CHARGE:
      pft_->backend->fc_calibpulse();
      break;
    default:
      // external - l1a triggered elsewhere
      break;
  }

  // read raw event data from polarfire
  std::vector<uint32_t> event_data_words = pft_->daqReadEvent();

  // cut data words into bytes
  const uint8_t *ptr = reinterpret_cast<const uint8_t*>(&event_data_words[0]);
  std::vector<uint8_t> event_data(ptr, ptr + sizeof(uint32_t)*event_data_words.size());

  // wrap event data in rogue frame to send it to file writer
  auto size = sizeof(uint32_t)*event_data.size();
  auto frame = reqFrame(size, true);
  frame->setPayload(size);
  std::copy(event_data.begin(), event_data.end(), frame->begin());
  sendFrame(frame);

  // wrap data in eudaq object and send it downstream to the monitoring
  auto ev = eudaq::Event::MakeUnique("HgcrocRaw");
  ev->AddBlock(fpga_id_, event_data);
  SendEvent(std::move(ev));
}

