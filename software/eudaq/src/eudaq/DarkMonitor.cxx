#include "eudaq/DarkMonitor.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include <ratio>
#include <thread>
#include "TH1.h"
#include "TH2.h"

namespace {
auto dummy0 = eudaq::Factory<eudaq::Monitor>::Register<
    eudaq::DarkMonitor, const std::string &, const std::string &>(
    eudaq::DarkMonitor::factory_id_);
}

namespace eudaq {

void DarkMonitor::AtConfiguration(){
  int nPlanes = 19;
  eventNum = 0;
  adc_thresh = -1;

  hcalEventPlane0 = m_monitor->Book<TH2D>("Event/Plane00", "Plane00", "Plane 0", "Plane0;Channel;Side of Bar",  2, 0., 2., 16, 0., 16.);
  hcalEventPlane1 = m_monitor->Book<TH2D>("Event/Plane01", "Plane01", "Plane 1", "Plane1;Channel;Side of Bar", 16, 0., 16., 2, 0., 2.);
  hcalEventPlane2 = m_monitor->Book<TH2D>("Event/Plane02", "Plane02", "Plane 2", "Plane2;Channel;Side of Bar",  2, 0., 2, 16, 0., 16.);
  hcalEventPlane3 = m_monitor->Book<TH2D>("Event/Plane03", "Plane03", "Plane 3", "Plane3;Channel;Side of Bar", 16, 0., 16., 2, 0., 2.);
  hcalEventPlane4 = m_monitor->Book<TH2D>("Event/Plane04", "Plane04", "Plane 4", "Plane4;Channel;Side of Bar",  2, 0., 2., 16, 0., 16.);
  hcalEventPlane5 = m_monitor->Book<TH2D>("Event/Plane05", "Plane05", "Plane 5", "Plane5;Channel;Side of Bar", 16, 0., 16., 2, 0., 2.);
  hcalEventPlane6 = m_monitor->Book<TH2D>("Event/Plane06", "Plane06", "Plane 6", "Plane6;Channel;Side of Bar",  2, 0., 2., 16, 0., 16.);
  hcalEventPlane7 = m_monitor->Book<TH2D>("Event/Plane07", "Plane07", "Plane 7", "Plane7;Channel;Side of Bar", 16, 0., 16., 2, 0., 2.);
  hcalEventPlane8 = m_monitor->Book<TH2D>("Event/Plane08", "Plane08", "Plane 8", "Plane8;Channel;Side of Bar",  2, 0., 2., 16, 0., 16.);
  hcalEventPlane9 = m_monitor->Book<TH2D>("Event/Plane09", "Plane09", "Plane 9", "Plane9;Channel;Side of Bar", 24, 0., 24., 2, 0., 2.);
  hcalEventPlane10 = m_monitor->Book<TH2D>("Event/Plane10", "Plane10", "Plane 10", "Plane10;Channel;Side of Bar",  2, 0., 2., 24, 0., 24.);
  hcalEventPlane11 = m_monitor->Book<TH2D>("Event/Plane11", "Plane11", "Plane 11", "Plane11;Channel;Side of Bar", 24, 0., 24., 2, 0., 2.);
  hcalEventPlane12 = m_monitor->Book<TH2D>("Event/Plane12", "Plane12", "Plane 12", "Plane12;Channel;Side of Bar",  2, 0., 2., 24, 0., 24.);
  hcalEventPlane13 = m_monitor->Book<TH2D>("Event/Plane13", "Plane13", "Plane 13", "Plane13;Channel;Side of Bar", 24, 0., 24., 2, 0., 2.);
  hcalEventPlane14 = m_monitor->Book<TH2D>("Event/Plane14", "Plane14", "Plane 14", "Plane14;Channel;Side of Bar",  2, 0., 2., 24, 0., 24.);
  hcalEventPlane15 = m_monitor->Book<TH2D>("Event/Plane15", "Plane15", "Plane 15", "Plane15;Channel;Side of Bar", 24, 0., 24., 2, 0., 2.);
  hcalEventPlane16 = m_monitor->Book<TH2D>("Event/Plane16", "Plane16", "Plane 16", "Plane16;Channel;Side of Bar",  2, 0., 2., 24, 0., 24.);
  hcalEventPlane17 = m_monitor->Book<TH2D>("Event/Plane17", "Plane17", "Plane 17", "Plane17;Channel;Side of Bar", 24, 0., 24., 2, 0., 2.);
  hcalEventPlane18 = m_monitor->Book<TH2D>("Event/Plane18", "Plane18", "Plane 18", "Plane18;Channel;Side of Bar",  2, 0., 2., 24, 0., 24.);


  hcalRunPlane0 = m_monitor->Book<TH2D>("Run/Plane00", "Plane00", "Plane 0", "Plane0;Channel;Side of Bar",  2, 0., 2., 16, 0., 16.);
  hcalRunPlane1 = m_monitor->Book<TH2D>("Run/Plane01", "Plane01", "Plane 1", "Plane1;Channel;Side of Bar", 16, 0., 16., 2, 0., 2.);
  hcalRunPlane2 = m_monitor->Book<TH2D>("Run/Plane02", "Plane02", "Plane 2", "Plane2;Channel;Side of Bar",  2, 0., 2., 16, 0., 16.);
  hcalRunPlane3 = m_monitor->Book<TH2D>("Run/Plane03", "Plane03", "Plane 3", "Plane3;Channel;Side of Bar", 16, 0., 16., 2, 0., 2.);
  hcalRunPlane4 = m_monitor->Book<TH2D>("Run/Plane04", "Plane04", "Plane 4", "Plane4;Channel;Side of Bar",  2, 0., 2., 16, 0., 16.);
  hcalRunPlane5 = m_monitor->Book<TH2D>("Run/Plane05", "Plane05", "Plane 5", "Plane5;Channel;Side of Bar", 16, 0., 16., 2, 0., 2.);
  hcalRunPlane6 = m_monitor->Book<TH2D>("Run/Plane06", "Plane06", "Plane 6", "Plane6;Channel;Side of Bar",  2, 0., 2., 16, 0., 16.);
  hcalRunPlane7 = m_monitor->Book<TH2D>("Run/Plane07", "Plane07", "Plane 7", "Plane7;Channel;Side of Bar", 16, 0., 16., 2, 0., 2.);
  hcalRunPlane8 = m_monitor->Book<TH2D>("Run/Plane08", "Plane08", "Plane 8", "Plane8;Channel;Side of Bar",  2, 0., 2., 16, 0., 16.);
  hcalRunPlane9 = m_monitor->Book<TH2D>("Run/Plane09", "Plane09", "Plane 9", "Plane9;Channel;Side of Bar", 24, 0., 24., 2, 0., 2.);
  hcalRunPlane10 = m_monitor->Book<TH2D>("Run/Plane10", "Plane10", "Plane 10", "Plane10;Channel;Side of Bar",  2, 0., 2., 24, 0., 24.);
  hcalRunPlane11 = m_monitor->Book<TH2D>("Run/Plane11", "Plane11", "Plane 11", "Plane11;Channel;Side of Bar", 24, 0., 24., 2, 0., 2.);
  hcalRunPlane12 = m_monitor->Book<TH2D>("Run/Plane12", "Plane12", "Plane 12", "Plane12;Channel;Side of Bar",  2, 0., 2., 24, 0., 24.);
  hcalRunPlane13 = m_monitor->Book<TH2D>("Run/Plane13", "Plane13", "Plane 13", "Plane13;Channel;Side of Bar", 24, 0., 24., 2, 0., 2.);
  hcalRunPlane14 = m_monitor->Book<TH2D>("Run/Plane14", "Plane14", "Plane 14", "Plane14;Channel;Side of Bar",  2, 0., 2., 24, 0., 24.);
  hcalRunPlane15 = m_monitor->Book<TH2D>("Run/Plane15", "Plane15", "Plane 15", "Plane15;Channel;Side of Bar", 24, 0., 24., 2, 0., 2.);
  hcalRunPlane16 = m_monitor->Book<TH2D>("Run/Plane16", "Plane16", "Plane 16", "Plane16;Channel;Side of Bar",  2, 0., 2., 24, 0., 24.);
  hcalRunPlane17 = m_monitor->Book<TH2D>("Run/Plane17", "Plane17", "Plane 17", "Plane17;Channel;Side of Bar", 24, 0., 24., 2, 0., 2.);
  hcalRunPlane18 = m_monitor->Book<TH2D>("Run/Plane18", "Plane18", "Plane 18", "Plane18;Channel;Side of Bar",  2, 0., 2., 24, 0., 24.);

  tsRun = m_monitor->Book<TH2D>("Run/TrigScint", "TrigScint", "TrigScint", "TrigScint;Channel", 1, 0., 1., 12, 0., 12.);
  tsEvent = m_monitor->Book<TH2D>("Event/TrigScint", "TrigScint", "TrigScint", "TrigScint;Channel", 1, 0., 1., 12, 0., 12.);

  m_monitor->SetDrawOptions(hcalEventPlane0, "COLZ");
  m_monitor->SetDrawOptions(hcalEventPlane1, "COLZ");
  m_monitor->SetDrawOptions(hcalEventPlane2, "COLZ");
  m_monitor->SetDrawOptions(hcalEventPlane3, "COLZ");
  m_monitor->SetDrawOptions(hcalEventPlane4, "COLZ");
  m_monitor->SetDrawOptions(hcalEventPlane5, "COLZ");
  m_monitor->SetDrawOptions(hcalEventPlane6, "COLZ");
  m_monitor->SetDrawOptions(hcalEventPlane7, "COLZ");
  m_monitor->SetDrawOptions(hcalEventPlane8, "COLZ");
  m_monitor->SetDrawOptions(hcalEventPlane9, "COLZ");
  m_monitor->SetDrawOptions(hcalEventPlane10, "COLZ");
  m_monitor->SetDrawOptions(hcalEventPlane11, "COLZ");
  m_monitor->SetDrawOptions(hcalEventPlane12, "COLZ");
  m_monitor->SetDrawOptions(hcalEventPlane13, "COLZ");
  m_monitor->SetDrawOptions(hcalEventPlane14, "COLZ");
  m_monitor->SetDrawOptions(hcalEventPlane15, "COLZ");
  m_monitor->SetDrawOptions(hcalEventPlane16, "COLZ");
  m_monitor->SetDrawOptions(hcalEventPlane17, "COLZ");
  m_monitor->SetDrawOptions(hcalEventPlane18, "COLZ");
  m_monitor->SetDrawOptions(hcalRunPlane0, "COLZ");
  m_monitor->SetDrawOptions(hcalRunPlane1, "COLZ");
  m_monitor->SetDrawOptions(hcalRunPlane2, "COLZ");
  m_monitor->SetDrawOptions(hcalRunPlane3, "COLZ");
  m_monitor->SetDrawOptions(hcalRunPlane4, "COLZ");
  m_monitor->SetDrawOptions(hcalRunPlane5, "COLZ");
  m_monitor->SetDrawOptions(hcalRunPlane6, "COLZ");
  m_monitor->SetDrawOptions(hcalRunPlane7, "COLZ");
  m_monitor->SetDrawOptions(hcalRunPlane8, "COLZ");
  m_monitor->SetDrawOptions(hcalRunPlane9, "COLZ");
  m_monitor->SetDrawOptions(hcalRunPlane10, "COLZ");
  m_monitor->SetDrawOptions(hcalRunPlane11, "COLZ");
  m_monitor->SetDrawOptions(hcalRunPlane12, "COLZ");
  m_monitor->SetDrawOptions(hcalRunPlane13, "COLZ");
  m_monitor->SetDrawOptions(hcalRunPlane14, "COLZ");
  m_monitor->SetDrawOptions(hcalRunPlane15, "COLZ");
  m_monitor->SetDrawOptions(hcalRunPlane16, "COLZ");
  m_monitor->SetDrawOptions(hcalRunPlane17, "COLZ");
  m_monitor->SetDrawOptions(hcalRunPlane18, "COLZ");
  m_monitor->SetDrawOptions(tsEvent, "COLZ");
  m_monitor->SetDrawOptions(tsRun, "COLZ");

  std::cout << "NEW PLOTTTTTTT!!!!!!!" << std::endl;
}

void DarkMonitor::AtEventReception(EventSP ev) {
	eventNum++;
	//std::cout << "MONITOOOOOOORING" << std::endl;
  //std::cout << ev->GetDeviceN() << "  " << ev->GetDescription() << std::endl;
	//ev->Print(std::cout);
  ev->GetBlock(0x1);
  std::vector<uint8_t> data{ev->GetBlock(0x1)};

	auto size{data.size() - 8 - 4};
  auto roc_subpacket_size{42*4};
  auto start{8 + size%roc_subpacket_size + 2};
  auto roc_id{data[start + 1] << 8 | data[start]};
  start += 14;
  auto tdc{getField(data[start + 1] << 8 | data[start], 9, 0)};

	/*std::cout << "size " << size << std::endl;
	std::cout << "roc_subpacket_size " << roc_subpacket_size << std::endl;
	std::cout << "start " << start << std::endl;
	std::cout << "roc_id " << roc_id << std::endl;
	std::cout << "tdc " << tdc << std::endl;
  std::cout << ev->GetEventID() << std::endl;*/

	bool newEvent = false; //update with flag for new spill
	if(newEvent){
	  hcalEventPlane0->Reset();
	  hcalEventPlane1->Reset();
	  hcalEventPlane2->Reset();
	  hcalEventPlane3->Reset();
	  hcalEventPlane4->Reset();
	  hcalEventPlane5->Reset();
	  hcalEventPlane6->Reset();
	  hcalEventPlane7->Reset();
	  hcalEventPlane8->Reset();
	  hcalEventPlane9->Reset();
	  hcalEventPlane10->Reset();
	  hcalEventPlane11->Reset();
	  hcalEventPlane12->Reset();
	  hcalEventPlane13->Reset();
	  hcalEventPlane14->Reset();
	  hcalEventPlane15->Reset();
	  hcalEventPlane16->Reset();
	  hcalEventPlane17->Reset();
	  hcalEventPlane18->Reset();
	  tsEvent->Reset();
	}

	if (ev->GetDescription().compare("HCalTestBeamRaw") == 0) {
          int plane = 0; //getPlane
          int bar = 0; //getBar
          int end = 0; //getEnd
          int chan = 0; //getChannel
          int adc = 0;//getADC

          if(adc < adc_thresh){
            continue;
          }
          if(plane == 0){
            hcalEventPlane0->Fill(end, chan);
            hcalRunPlane0->Fill(end, chan);
          }else if(plane == 1){
            hcalEventPlane1->Fill(chan, end);
            hcalRunPlane1->Fill(chan, end);
          }
          else if(plane == 2){
            hcalEventPlane2->Fill(end, chan);
            hcalRunPlane2->Fill(end, chan);
          }
          else if(plane == 3){
            hcalEventPlane3->Fill(chan, end);
            hcalRunPlane3->Fill(chan, end);
          }
          else if(plane == 4){
            hcalEventPlane4->Fill(end, chan);
            hcalRunPlane4->Fill(end, chan);
          }
          else if(plane == 5){
            hcalEventPlane5->Fill(chan, end);
            hcalRunPlane5->Fill(chan, end);
          }
          else if(plane == 6){
            hcalEventPlane6->Fill(end, chan);
            hcalRunPlane6->Fill(end, chan);
          }
          else if(plane == 7){
            hcalEventPlane7->Fill(chan, end);
            hcalRunPlane7->Fill(chan, end);
          }
          else if(plane == 8){
            hcalEventPlane8->Fill(end, chan);
            hcalRunPlane8->Fill(end, chan);
          }
          else if(plane == 9){
            hcalEventPlane9->Fill(chan, end);
            hcalRunPlane9->Fill(chan, end);
          }
          else if(plane == 10){
            hcalEventPlane10->Fill(end, chan);
            hcalRunPlane10->Fill(end, chan);
          }
          else if(plane == 11){
            hcalEventPlane11->Fill(chan, end);
            hcalRunPlane11->Fill(chan, end);
          }
          else if(plane == 12){
            hcalEventPlane12->Fill(end, chan);
            hcalRunPlane12->Fill(end, chan);
          }
          else if(plane == 13){
            hcalEventPlane13->Fill(chan, end);
            hcalRunPlane13->Fill(chan, end);
          }
          else if(plane == 14){
            hcalEventPlane14->Fill(end, chan);
            hcalRunPlane14->Fill(end, chan);
          }
          else if(plane == 15){
            hcalEventPlane15->Fill(chan, end);
            hcalRunPlane15->Fill(chan, end);
          }
          else if(plane == 16){
            hcalEventPlane16->Fill(end, chan);
            hcalRunPlane16->Fill(end, chan);
          }
          else if(plane == 17){
            hcalEventPlane17->Fill(chan, end);
            hcalRunPlane17->Fill(chan, end);
          }
          else if(plane == 18){
            hcalEventPlane18->Fill(end, chan);
            hcalRunPlane18->Fill(end, chan);
          }
	} else if (ev->GetDeviceN() == 0x2) {
    int chan = 0; //Get TS Channel
	  tsEvent->Fill(0., chan);
	  tsRun->Fill(0., chan);
	}
}
} // namespace eudaq
