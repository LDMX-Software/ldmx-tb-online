#ifndef EUDAQ_DARKRUNCONTROL_H
#define EUDAQ_DARKRUNCONTROL_H

//---< eudaq >---//
#include "eudaq/RunControl.hh"

namespace eudaq { 
class DarkRunControl : public eudaq::RunControl {
public:
  /// Constructor 
  DarkRunControl(const std::string &listenaddress);
  /**
   */
  void Configure() override;
  
  /**
   */
  void StartRun() override;
  
  /**
   */
  void StopRun() override;
  
  /**
   */
  void Exec() override;
  
  static const uint32_t m_id_factory = eudaq::cstr2hash("DarkRunControl");

private:

  ///
  uint32_t m_stop_second;
  
  ///
  bool m_flag_running;
  
  ///
  std::chrono::steady_clock::time_point m_tp_start_run;
};
}
#endif // EUDAQ_DARKRUNCONTROL_H
