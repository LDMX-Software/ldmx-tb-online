#include "eudaq/Producer.hh"
#include <iostream>
#include <memory>

#include "pflib/Hcal.h"
#include "pflib/Backend.h"
#include "pflib/WishboneInterface.h"

/**
 * Interface to a single polarfire 
 */
class PolarFireInterface : public eudaq::Producer {
 public:
  PolarFireInterface(const std::string & name, const std::string & runcontrol);
  void DoInitialise() override;
  void DoConfigure() override;
  void DoStartRun() override;
  void DoStopRun() override;
  void DoTerminate() override;
  void DoReset() override;
  void RunLoop() override;
  
  static const uint32_t factory_id_ = eudaq::cstr2hash("PolarFireInterface");
 private:
  std::shared_ptr<pflib::WishboneInterface> wb_;
  std::shared_ptr<pflib::Backend> backend_;
  std::unique_ptr<pflib::Hcal> hcal_;
};

