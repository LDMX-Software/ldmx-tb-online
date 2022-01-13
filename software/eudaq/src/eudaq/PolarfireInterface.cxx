#include "eudaq/PolarfireInterface.h"

#include "pflib/ROC.h"
#include "pflib/Elinks.h"
#include "pflib/Bias.h"
#include "pflib/FastControl.h"
#include "pflib/rogue/RogueWishboneInterface.h"

/**
 * This code registers our class with the central eudaq factory
 * so that it can be constructed when needed
 */
namespace {
  auto dummy0 = eudaq::Factory<eudaq::Producer>::
    Register<PolarFireInterface, 
             const std::string&, 
             const std::string&>(PolarFireInterface::factory_id_);
}

PolarFireInterface::PolarFireInterface(const std::string & name, const std::string & runcontrol)
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
void PolarFireInterface::DoInitialise(){
  auto ini = GetInitConfiguration();

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
void PolarFireInterface::DoConfigure(){
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
void PolarFireInterface::DoStartRun(){
}
/**
 * Clean close
 */
void PolarFireInterface::DoStopRun(){
}
/**
 * typically used in failure modes, check out state function
 * i.e. recover from failure
 */
void PolarFireInterface::DoReset(){
}
/**
 * Not sure what this does...
 */
void PolarFireInterface::DoTerminate(){
}
/**
 * Include some "mode" about if sending our own L1A ("local" mode)
 * or some "external" mode where L1A is generated elsewhere
 */
void PolarFireInterface::RunLoop(){
  /*
  auto ev = eudaq::Event::MakeUnique("Ex0Raw");
  ev->SetTag("Plane ID", std::to_string(m_plane_id));
  ev->SetTimestamp(du_ts_beg_ns.count(), du_ts_end_ns.count());
  ev->SetTriggerN(trigger_n);
  ev->AddBlock(block_id, data);
  SendEvent(std::move(ev));
  */
}
