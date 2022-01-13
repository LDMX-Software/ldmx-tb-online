#ifndef EUDAQ_TESTBEAMDATACOLLECTOR_H
#define EUDAQ_TESTBEAMDATACOLLECTOR_H

//---< eudaq >---//
#include "eudaq/DataCollector.hh"

namespace eudaq {
class TestBeamDataCollector : public DataCollector {
public:
  using DataCollector::DataCollector;
  void DoConfigure() override;
  void DoReceive(ConnectionSPC id, EventSP ev) override;

  /// ID used to register this data collector with the eudaq environment
  static const uint32_t factory_id_{eudaq::cstr2hash("TestBeamDataCollector")};

private:
};
} // namespace eudaq

#endif // EUDAQ_TESTBEAMDATACOLLECTOR_H
