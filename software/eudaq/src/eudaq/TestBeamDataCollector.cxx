#include "eudaq/TestBeamDataCollector.h"

#include <iostream>

namespace {
auto dummy0 = eudaq::Factory<eudaq::DataCollector>::Register<
    eudaq::TestBeamDataCollector, const std::string &, const std::string &>(
    eudaq::TestBeamDataCollector::factory_id_);
}

namespace eudaq {
void TestBeamDataCollector::DoConfigure() {
  /*m_noprint = 0;
  auto conf = GetConfiguration();
  if (conf) {
    conf->Print();
    m_noprint = conf->Get("DISABLE_PRINT", 0);
  }*/
}
  
void TestBeamDataCollector::DoReceive(ConnectionSPC id, EventSP ev) {
  // if (!m_noprint)
  //std::cout << "I'm here" << std::endl;
  //ev->Print(std::cout);  
  //WriteEvent(ev);

  try {
    if(ev->IsBORE()){
      if(GetConfiguration())
	ev->SetTag("EUDAQ_CONFIG", to_string(*GetConfiguration()));
      if(GetInitConfiguration())
	ev->SetTag("EUDAQ_CONFIG_INIT", to_string(*GetInitConfiguration()));
    }
    
    ev->SetRunN(GetRunNumber());
    ev->SetEventN(m_evt_c);
    m_evt_c ++;
    ev->SetStreamN(m_dct_n);
    std::unique_lock<std::mutex> lk(m_mtx_sender);
    auto senders = m_senders;
    lk.unlock();
    if(m_evt_c%m_fraction != 0){
      return;
    }
    for(auto &e: senders){
      if(e.second)
	e.second->SendEvent(ev);
      else
	EUDAQ_THROW("DataCollector::WriterEvent, using a null pointer of DataSender");
    }
    
  } catch (const Exception &e) { // try
    std::string msg = "Exception to file: ";
    msg += e.what();
    EUDAQ_ERROR(msg);
    SetStatus(Status::STATE_ERROR, msg);
  }
}
  
} // namespace eudaq
