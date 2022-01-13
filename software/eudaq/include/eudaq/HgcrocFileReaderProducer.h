#ifndef EUDAQ_HGCROCFILEREADERPRODUCER_H
#define EUDAQ_HGCROCFILEREADERPRODUCER_H

//---< C++ StdLib >---//
#include <fstream>

//---< eudaq >---//
#include "eudaq/Producer.hh"

namespace eudaq {
class HgcrocFileReaderProducer : public eudaq::Producer {
public:
  /**
   * Constructor
   */
  HgcrocFileReaderProducer(const std::string &name,
                         const std::string &runcontrol);
  /**
   */
  void DoInitialise() override;

  /**
   */
  void DoConfigure() override;

  /**
   */
  void DoStartRun() override;

  /**
   */
  void DoStopRun() override;

  /**
   */
  void DoTerminate() override;

  /**
   */
  void DoReset() override;

  /**
   */
  void RunLoop() override;

  /// ID used to register this producer with the eudaq environment
  static const uint32_t factory_id_{eudaq::cstr2hash("HgcrocFileReaderProducer")};

private:
  /// File reader
  std::shared_ptr<std::ifstream> ifile; 

};
} // namespace eudaq

#endif // EUDAQ_HGCROCFILEREADERPRODUCER_H
