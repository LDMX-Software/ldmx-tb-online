//#include "eudaq/PolarfireProducer.h"

#include "eudaq/Producer.hh"
#include <iostream>
#include <memory>

#include "pflib/Hcal.h"
#include "pflib/Backend.h"
#include "pflib/WishboneInterface.h"
#include "pflib/ROC.h"
#include "pflib/Elinks.h"
#include "pflib/Bias.h"
#include "pflib/FastControl.h"
#include "pflib/rogue/RogueWishboneInterface.h"

/**
 * Interface to a single polarfire 
 */
class PolarfireProducer : public eudaq::Producer {
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
  std::shared_ptr<pflib::WishboneInterface> wb_;
  std::shared_ptr<pflib::Backend> backend_;
  std::unique_ptr<pflib::Hcal> hcal_;
};

/**
 * This code registers our class with the central eudaq factory
 * so that it can be constructed when needed
 */
namespace {
auto dummy_pfint = eudaq::Factory<eudaq::Producer>::Register<
    PolarfireProducer, const std::string&, const std::string&>(
    PolarfireProducer::factory_id_);
}

PolarfireProducer::PolarfireProducer(const std::string & name, const std::string & runcontrol)
  : eudaq::Producer(name, runcontrol) {}

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

  // debug printout
  ini->Print(std::cout);

  auto addr{ini->Get("TCP_ADDR", "127.0.0.1")};
  auto port{ini->Get("TCP_PORT", 8000)};
  EUDAQ_INFO("TCP client listening on " + addr + ":" + std::to_string(port));

  auto ro_mode{ini->Get("RO_MODE", "MemMap")};
  EUDAQ_INFO("Readout Mode set to " + ro_mode);

  if (ro_mode == "MemMap") {
    auto ptr{std::make_shared<pflib::rogue::RogueWishboneInterface>(addr,port)};
    wb_ = ptr;
    backend_ = ptr;
  } else {
    EUDAQ_THROW("Unrecognized readout mode "+ro_mode);
  }

  hcal_ = std::make_unique<pflib::Hcal>(wb_.get());
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

  // debug printout
  conf->Print(std::cout);

  // or in independent config file?
  auto fpga_id = conf->Get("FPGA_ID", 0);

  auto& daq = hcal_->daq();
}
/**
 * Start of run, softer reset to start cleanly
 *  - Jeremy's branch
 *  - flags for "external" or various "local" modes
 */
void PolarfireProducer::DoStartRun(){
}
/**
 * Clean close
 */
void PolarfireProducer::DoStopRun(){
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
  /*
  auto ev = eudaq::Event::MakeUnique("Ex0Raw");
  ev->SetTag("Plane ID", std::to_string(m_plane_id));
  ev->SetTimestamp(du_ts_beg_ns.count(), du_ts_end_ns.count());
  ev->SetTriggerN(trigger_n);
  ev->AddBlock(block_id, data);
  SendEvent(std::move(ev));
  */
}

