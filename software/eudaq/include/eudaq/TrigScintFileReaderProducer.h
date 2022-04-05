#pragma once

//---< C++ >---//
#include <fstream>

//---< eudaq >---//
#include "eudaq/Producer.hh"

namespace eudaq { 

class TrigScintFileReaderProducer : public eudaq::Producer {
 public:
  /**
   * Constructor
   */
  TrigScintFileReaderProducer(const std::string &name, const std::string &runcontrol);

  ///
  void DoInitialise() override;

  ///
  void DoConfigure() override;

  ///
  void DoStartRun() override;

  ///
  void DoStopRun() override;

  ///
  void DoTerminate() override;

  ///
  void DoReset() override;

  ///
  void RunLoop() override;
  
  /// ID used to register this producer with the eudaq environment
  static const uint32_t factory_id_{eudaq::cstr2hash("TrigScintFileReaderProducer")};

 private:
  /// Function to convert hex to binary
  uint64_t hex2Bin(const std::string& shex); 

  /// File reader
  std::shared_ptr<std::ifstream> ifile; 

};
}
