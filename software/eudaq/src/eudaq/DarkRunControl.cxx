
#include "eudaq/DarkRunControl.h" 

namespace{
  auto dummy0 = eudaq::Factory<eudaq::RunControl>::
    Register<eudaq::DarkRunControl, const std::string&>(eudaq::DarkRunControl::m_id_factory);
}

namespace eudaq { 

DarkRunControl::DarkRunControl(const std::string & listenaddress)
  :RunControl(listenaddress){
  m_flag_running = false;
}

void DarkRunControl::StartRun(){
  RunControl::StartRun();
  m_tp_start_run = std::chrono::steady_clock::now();
  m_flag_running = true;
}

void DarkRunControl::StopRun(){
  RunControl::StopRun();
  m_flag_running = false;
}

void DarkRunControl::Configure(){
  auto conf = GetConfiguration();
  m_stop_second = conf->Get("STOP_RUN_AFTER_N_SECONDS", 0);
  RunControl::Configure();
}

void DarkRunControl::Exec(){
  StartRunControl();
  while(IsActiveRunControl()){
    if(m_flag_running && m_stop_second){
      auto tp_now = std::chrono::steady_clock::now();
      std::chrono::nanoseconds du_ts(tp_now - m_tp_start_run);
      if(du_ts.count()/1000000000>m_stop_second)
	StopRun();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
}
}
