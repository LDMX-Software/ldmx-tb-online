
#include <memory>
#include <chrono>

#include <rogue/utilities/fileio/StreamWriter.h>
#include <rogue/utilities/fileio/StreamWriterChannel.h>
#include <rogue/interfaces/stream/Master.h>
#include <rogue/interfaces/stream/Slave.h>
#include <rogue/interfaces/stream/TcpClient.h>
#include <rogue/interfaces/stream/Frame.h>
#include <rogue/interfaces/stream/FrameIterator.h>
#include <rogue/interfaces/stream/FrameLock.h>

#include <eudaq/Producer.hh>

#include "eudaq/RogueDataSender.h"

#include "pflib/PolarfireTarget.h"
#include "pflib/rogue/RogueWishboneInterface.h"


/**
 * Interface to a single polarfire 
 */
class PolarfireProducer : public eudaq::Producer, 
                          public rogue::interfaces::stream::Master {
  /**
   * Rogue data sender translates a received rogue frame into
   * an eudaq event. This is used during DMA readout.
   */
  class DMADataSender : public eudaq::RogueDataSender {
   public:
    DMADataSender(eudaq::Producer *producer) : RogueDataSender(producer){};
    ~DMADataSender() = default;
    virtual void sendEvent(std::shared_ptr<rogue::interfaces::stream::Frame> frame) final override {
      static std::size_t i_frame{0};
      auto lock = frame->lock();
      std::vector<uint8_t> event_data{frame->begin(), frame->end()};
      // wrap data in eudaq object and send it downstream to the monitoring
      auto ev = eudaq::Event::MakeUnique(static_cast<PolarfireProducer*>(producer_)->event_name());
      ev->AddBlock(static_cast<PolarfireProducer*>(producer_)->block(), event_data); // fpga ID?
      producer_->SendEvent(std::move(ev));
    }
  };
 public:
  PolarfireProducer(const std::string & name, const std::string & runcontrol);
  void DoInitialise() override;
  void DoConfigure() override;
  void DoStartRun() override;
  void DoStopRun() override;
  void DoTerminate() override;
  void DoReset() override;
  /// put onto its own tread by eudaq::CommandReceiver
  void RunLoop() override;
  static const uint32_t factory_id_ = eudaq::cstr2hash("PolarfireProducer");
  /**
   * the block in the event we should put the data
   * unclear on if this should always be the same or if it should be
   * the id of the polarfire producer
   */
  int block() const {
    return fpga_id_;
  }
  /**
   * name of the event we creat,
   * unclear on if this is important for event aligning or not
   */
  std::string event_name() const {
    return "PolarfireRaw";
  }
 private:
  /// pass end-of-run to separate thread
  bool exiting_run_;
  /// milliseconds the polarfire is busy
  std::chrono::milliseconds pf_busy_ms_;
 private:
  /// connection to polarfire
  std::unique_ptr<pflib::PolarfireTarget> pft_;
  /// get the RogueWishboneInterface handle
  pflib::rogue::RogueWishboneInterface* rwbi() {
    return dynamic_cast<pflib::rogue::RogueWishboneInterface*>(pft_->wb);
  }
  /// how daq event reading is triggered
  enum class L1A_MODE {
    PEDESTAL,
    CHARGE,
    EXTERNAL
  } the_l1a_mode_;
  /// has DMA readout been enabled?
  bool dma_enabled_;
  /// dma data sender to bring data from rogue into eudaq
  std::shared_ptr<DMADataSender> dma_sender_;
  /// rogue writer for non-DMA readout
  std::shared_ptr<rogue::utilities::fileio::StreamWriter> nondma_writer_;
  /// host that we should be watching
  std::string host_;
  /// port of **wishbone** server on that host (DMA server is assumed to be +2)
  int port_;
  /// ID for fpga connected to this polarfire
  int fpga_id_;
  /// output file path
  std::string output_path_;
  /// file prefix (suffix is number generated by file writer)
  std::string file_prefix_{"ldmx_hcal"};
  /// amount of bytes to buffer in memory
  int file_buffer_size_;
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
    rogue::interfaces::stream::Master(),
    nondma_writer_{rogue::utilities::fileio::StreamWriter::create()} {}

/**
 * Initialization procedures
 *
 * We open the connection to the wishbone interface and pass
 * this interface connection to our PolarfireTarget object which holds 
 * more of the "high-level" functions. We also open the connection
 * to the backend. With Rogue, the wishbone interface and
 * the backend are the same.
 */
void PolarfireProducer::DoInitialise() try {
  auto ini = GetInitConfiguration();

  host_ = ini->Get("TCP_ADDR", "127.0.0.1");
  port_ = ini->Get("TCP_PORT", 8000);
  pft_ = std::make_unique<pflib::PolarfireTarget>(
      new pflib::rogue::RogueWishboneInterface(host_,port_));
  EUDAQ_INFO("TCP client listening on " + host_ + ":" + std::to_string(port_));
} catch (const pflib::Exception& e) {
  EUDAQ_THROW("PFLIB ["+e.name()+"] : "+e.message());
}

/**
 * Configuration of connection to polarfire
 *
 * Besides the trigger type for the run and the configuratino
 * of the output raw files, all of the configuration of the
 * chip must be done _prior_ to launching this producer within
 * pftool.
 *
 * ## Cases to Handle
 *  - instance of this class for each polarfire in real world
 *  - specify which polarfire we are
 *  - specify which ROCs are actually there
 *  - every ROC has its own config file
 *  - beam runs, charge injection runs, ...
 *  - e.g. set charge injection amplitude from here
 *  - (pflib) get links up and running properly
 */
void PolarfireProducer::DoConfigure() try {
  auto conf = GetConfiguration();

  // expected number of milliseconds the PF is busy
  int pf_ro_rate{conf->Get("PF_READOUT_RATE", 100)};
  pf_busy_ms_ = std::chrono::milliseconds(1000/pf_ro_rate);
  EUDAQ_INFO("Readout rate set to "+std::to_string(pf_ro_rate)
      +"Hz => "+std::to_string(pf_busy_ms_.count())+"ms");

  // output file writing configuration
  output_path_ = conf->Get("OUTPUT_PATH",".");
  EUDAQ_INFO("Files written to diretory "+output_path_);
  file_prefix_ = conf->Get("FILE_PREFIX","ldmx_hcal");
  EUDAQ_INFO("Files will start with "+file_prefix_);

  auto l1a_mode{conf->Get("L1A_MODE","PEDESTAL")};
  if (l1a_mode == "PEDESTAL") {
    the_l1a_mode_ = L1A_MODE::PEDESTAL;
  } else if (l1a_mode == "CHARGE") {
    the_l1a_mode_ = L1A_MODE::CHARGE;
  } else if (l1a_mode == "EXTERNAL") {
    the_l1a_mode_ = L1A_MODE::EXTERNAL;
  } else {
    EUDAQ_THROW("Unrecognized L1A mode "+l1a_mode);
  }
  EUDAQ_INFO("L1A Trigger Mode set to " + l1a_mode);

  auto& daq = pft_->hcal.daq();
  auto& elinks = pft_->hcal.elinks();
  /****************************************************************************
   * ELINKS menu in pftool
   *    RELINK
   *    or manual DELAY and BITSLIP values
  if (conf->Get("ELINKS_DO_RELINK",true)) {
    pft_->elink_relink(2);
  } else {
    for (int i{0}; i < elinks.nlinks(); i++) {
      elinks.setBitslipAuto(i,false);
      elinks.setBitslip(i,
          conf->Get("ELINK_"+std::to_string(i)+"_BITSLIP", elinks.getBitslip(i)));
      elinks.setDelay(i,
          conf->Get("ELINK_"+std::to_string(i)+"_DELAY", 128));
    }
  }
   ***************************************************************************/

  /****************************************************************************
   * DAQ.SETUP menu in pftool commands
   *    FPGA
   *    DMA
   *    STANDARD 
   *    L1APARAMS
   *    MULTISAMPLE
  daq.setIds(fpga_id_);

  fpga_id_;  = conf->Get("FPGA_ID", 0);
  uint8_t samples_per_event = conf->Get("SAMPLES_PER_EVENT", 5);
  dma_enabled_ = conf->Get("DO_DMA_RO",true);
  if (dma_enabled_) {
    rwbi()->daq_dma_enable(dma_enabled_);
    rwbi()->daq_dma_setup((uint8_t)fpga_id_, (uint8_t)samples_per_event);
  }

  for (int i{0}; i < elinks.nlinks(); i++) {
    bool active{conf->Get("LINK_"+std::to_string(i)+"_ACTIVE",false)};
    elinks.markActive(i,active);
    if (elinks.isActive(i)) {
      daq.setupLink(i,false,false,15,40);
      int delay{conf->Get("LINK_"+std::to_string(i)+"_L1A_DELAY", 15)};
      int capture{conf->Get("LINK_"+std::to_string(i)+"_L1A_LENGTH", 40)};
      uint32_t reg = ((delay&0xff)<<8)|((capture&0xff)<<16);
      pft_->wb->wb_write(pflib::tgt_DAQ_Inbuffer,(i << 7)|1, reg);
    } else {
      // fully zero suppress this link
      daq.setupLink(i,true,true,15,40);
    }
  }
   ***************************************************************************/

  /****************************************************************************
   * ROC menu in pftool
   *    HARDRESET
   *    RESYNCLOAD
   *    LOAD_PARAM
  pft_->hcal.hardResetROCs(); // global reset
  pft_->hcal.resyncLoadROC(); // loops over ROCs if no i_roc provided
  for (int i{0}; i < 4; i++) {
    if (elinks.isActive(2*i) or elinks.isActive(2*i+1)) {
      // either or both of the links on this ROC are active
      pft_->loadROCParameters(i,
          conf->Get("ROC_"+std::to_string(i)+"_CONF_FILE_PATH",""),
          conf->Get("ROC_"+std::to_string(i)+"_PREPEND_DEFAULTS",true));
    }
  }
   ***************************************************************************/

  /* check chip if it is dma
   */
  uint8_t samples_per_event, fpga_id;
  rwbi()->daq_get_dma_setup(fpga_id, samples_per_event, dma_enabled_);
  fpga_id_ = fpga_id;
  EUDAQ_INFO("DMA Setup : "
      +std::string(dma_enabled_?" ENABLED":"DISABLED")
      +", FPGA = "+std::to_string(fpga_id)
      +", samples = "+std::to_string(samples_per_event));

  // construct pipeline depending on readout mode
  if (dma_enabled_) {
    dma_sender_ = std::make_shared<DMADataSender>(this);
    EUDAQ_DEBUG("DMADataSender created");
    rwbi()->daq_dma_dest(dma_sender_);
    EUDAQ_DEBUG("Receiver connected to TCP");
  } else {
    // we are the frame generator without DMA readout
    EUDAQ_DEBUG("non-DMA writer connected to us");
    this->addSlave(nondma_writer_->getChannel(0));
  }
} catch (const pflib::Exception& e) {
  EUDAQ_THROW("PFLIB ["+e.name()+"] : "+e.message());
}

/**
 * Start of run, softer reset to start cleanly
 *  - Jeremy's branch
 *  - flags for "external" or various "local" modes
 */
void PolarfireProducer::DoStartRun()  try {
  // set event tag for firmware
  std::time_t tt = time(NULL);
  std::tm gmtm = *std::gmtime(&tt); //GMT (UTC)
  pft_->backend->daq_setup_event_tag(GetRunNumber(), 
      gmtm.tm_mday, gmtm.tm_mon+1, gmtm.tm_hour, gmtm.tm_min);
  // construct output file path from directory, file prefix,
  // run, fpga, and timestamp
  std::stringstream output_file;
  output_file << output_path_ << "/" 
    << file_prefix_ 
    << "_fpga_" << fpga_id_
    << "_run_" << GetRunNumber() 
    << "_" << std::setfill('0') 
    << 1900+gmtm.tm_year 
    << std::setw(2) << 1+gmtm.tm_mon 
    << std::setw(2) << gmtm.tm_mday
    << "_" 
    << std::setw(2) << gmtm.tm_hour 
    << std::setw(2) << gmtm.tm_min 
    << std::setw(2) << gmtm.tm_sec
    << ".raw";
  // open file
  EUDAQ_INFO("Writing data stream to "+output_file.str());
  try {
    if (dma_enabled_) rwbi()->daq_dma_dest(output_file.str());
    else nondma_writer_->open(output_file.str());
  } catch (const std::exception& e) {
    EUDAQ_THROW("Rogue Error : "+std::string(e.what()));
  }
  // prep run
  EUDAQ_INFO("Preparing for new run.");
  pft_->prepareNewRun();
  // enable external triggers
  if (the_l1a_mode_ == L1A_MODE::EXTERNAL) {
    EUDAQ_INFO("Enabling external triggering");
    pft_->backend->fc_enables(true,true,false);
  }
  exiting_run_ = false;
} catch (const pflib::Exception& e) {
  EUDAQ_THROW("PFLIB ["+e.name()+"] : "+e.message());
}

/**
 * Clean close
 */
void PolarfireProducer::DoStopRun(){
  exiting_run_ = true;
  std::this_thread::sleep_for(2*pf_busy_ms_);
  if (the_l1a_mode_ == L1A_MODE::EXTERNAL)
    pft_->backend->fc_enables(false,true,false);
  if (dma_enabled_) rwbi()->daq_dma_close();
  else nondma_writer_->close();
}
/**
 * i.e. recover from failure, this is the only available
 * command if the producer ends up in an error state
 *
 * Go back to "newly created" essentially
 */
void PolarfireProducer::DoReset(){
  exiting_run_ = true;
  // LOCK_UN lock file?
  pft_.reset(nullptr);
  if (nondma_writer_->isOpen()) nondma_writer_->close();
}
/**
 * Not sure what this does...
 */
void PolarfireProducer::DoTerminate(){
  exiting_run_ = true;
  // close lock file?
}

/**
 * Include some "mode" about if sending our own L1A ("local" mode)
 * or some "external" mode where L1A is generated elsewhere
 *
 * This function is put onto its own thread by eudaq::CommandReceiver
 * when the run is started. This allows us to receive run control
 * commands without interfering with waiting for the polarfire to
 * respond to our wishbone commands.
 *
 * After setting up the writer to stream this data directly to disk,
 * we enter a loop which only exits when DoStopRun is called to modify
 * the member variable.
 */
void PolarfireProducer::RunLoop() try {
  // don't do anything if in external trigger mode
  if (dma_enabled_ and the_l1a_mode_ == L1A_MODE::EXTERNAL) {
    EUDAQ_INFO("DMA Readout and External trigger means nothing for RunLoop to do.");
    while (not exiting_run_) {
      int spill,occ,occ_max,vetoed,event;
      pft_->backend->fc_read_counters(spill,occ,occ_max,event,vetoed);
      EUDAQ_INFO("Read "+std::to_string(event)+" events according to FC");
      sleep(1);
    }
    return;
  }
  EUDAQ_INFO("Run loop beginning");
  /// loop until we receive end-of-run
  static std::size_t i_trig{0};
  while (not exiting_run_) {
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
    // start next window timer
    auto pf_trigger = std::chrono::steady_clock::now();
    i_trig++;
    auto pf_end_of_busy = pf_trigger + pf_busy_ms_;
    if (not dma_enabled_) {
      // without DMA enabled, need to PULL data from polarfire
      std::vector<uint32_t> event_data_words = pft_->daqReadEvent();
      // cut data words into bytes
      const uint8_t *ptr = reinterpret_cast<const uint8_t*>(&event_data_words[0]);
      std::vector<uint8_t> event_data(ptr, ptr + sizeof(uint32_t)*event_data_words.size());
      // wrap event data in rogue frame to send it to file writer
      auto size = event_data.size();
      auto frame = reqFrame(size, true);
      frame->setPayload(size);
      std::copy(event_data.begin(), event_data.end(), frame->begin());
      sendFrame(frame);
      // wrap data in eudaq object and send it downstream to the monitoring
      auto ev = eudaq::Event::MakeUnique(event_name());
      ev->AddBlock(block(), event_data);
      SendEvent(std::move(ev));
    }
    // wait current daq window is done
    std::this_thread::sleep_until(pf_end_of_busy);
  }
  EUDAQ_DEBUG("N Triggers Sent: " + std::to_string(i_trig));
} catch (const pflib::Exception& e) {
  EUDAQ_THROW("PFLIB ["+e.name()+"] : "+e.message());
}

