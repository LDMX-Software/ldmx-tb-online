
#include "TrigScintTestBeamMonitor.h"
#include "eudaq/TrigScintRawDecode.h"
#include <iostream>

namespace {
  auto dummy0 = eudaq::Factory<eudaq::Monitor>::Register<
    eudaq::TrigScintTestBeamMonitor, const std::string &, const std::string &>(
									       eudaq::TrigScintTestBeamMonitor::factory_id_);
}

namespace eudaq {

  void TrigScintTestBeamMonitor::AtConfiguration() {
    /**
     * Book plots here.
     */
    for( int i = 0 ; i < n_channels ; i++ ){

      h2_QvT.push_back( m_monitor->Book<TH2F>("h2_QvT_"+std::to_string(i),
					      "Q vs time (Chan "+std::to_string(i)+")",
					      ("h2_QvT_"+std::to_string(i)).data(),
					      ";time sample;Q",30,0,30,200,0,50000) );
      m_monitor->SetDrawOptions(h2_QvT[i],"colz");
      char temp[50];
      sprintf(temp,"Q vs time (Chan %i)",i);
      h2_QvT[i]->SetTitle(temp);

      
      
      h2_ADCvT.push_back( m_monitor->Book<TH2F>("h2_ADCvT_"+std::to_string(i),
						"ADC vs time (Chan "+std::to_string(i)+")",
						("h2_ADCvT_"+std::to_string(i)).data(),
						";time sample;ADC",30,0,30,256,0,256) );
      m_monitor->SetDrawOptions(h2_ADCvT[i],"colz");
      sprintf(temp,"ADC vs time (Chan %i)",i);
      h2_ADCvT[i]->SetTitle(temp);

      h2_QvT.push_back( m_monitor->Book<TH2F>("h2_QvT_"+std::to_string(i),
                                              "Q vs time (Chan "+std::to_string(i)+")",
                                              ("h2_QvT_"+std::to_string(i)).data(),
                                              ";time ple;Q",30,-0.5,29.5,200,0,50000) );
      m_monitor->SetDrawOptions(h2_QvT[i],"colz");
      sprintf(temp,"Q vs time (Chan %i)",i);
      h2_ADCvT[i]->SetTitle(temp);
      
      h2_TDCvT.push_back( m_monitor->Book<TH2F>("h2_TDCvT_"+std::to_string(i),
						"TDC vs time (Chan "+std::to_string(i)+")",
						("h2_TDCvT_"+std::to_string(i)).data(),
						";time sample;TDC",30,0,30,256,0,256) );
      m_monitor->SetDrawOptions(h2_TDCvT[i],"colz");
      sprintf(temp,"TDC vs time (Chan %i)",i);
      h2_TDCvT[i]->SetTitle(temp);
      h2_TDCvT[i]->GetXaxis()->SetTitle("Time Sample");
      h2_TDCvT[i]->GetYaxis()->SetTitle("TDC");

      h1_ADC.push_back( m_monitor->Book<TH1F>("h1_ADC_"+std::to_string(i),
					      "ADC Distribution (Chan "+std::to_string(i)+")",
					      ("h1_ADC_"+std::to_string(i)).data(),
					      ";ADC;Events",256,-0.5,255.5) );
      m_monitor->SetDrawOptions(h1_ADC[i],"colz");
      sprintf(temp,"ADC (Chan %i)",i);
      h1_ADC[i]->SetTitle(temp);

      h1_sumQ.push_back( m_monitor->Book<TH1F>("h1_sumQ_"+std::to_string(i),
					      "sumQ Distribution (Chan "+std::to_string(i)+")",
					      ("h1_sumQ_"+std::to_string(i)).data(),
					      ";Sum Q [fC];Events",200,0,5000) );
      m_monitor->SetDrawOptions(h1_sumQ[i],"colz");
      sprintf(temp,"Sum Q (Chan %i)",i);
      h1_sumQ[i]->SetTitle(temp);

      h1_sumQ_wide.push_back( m_monitor->Book<TH1F>("h1_sumQ_wide_"+std::to_string(i),
					      "sumQ Wide Distribution (Chan "+std::to_string(i)+")",
					      ("h1_sumQ_wide_"+std::to_string(i)).data(),
					      ";Sum Q [fC];Events",200,0,100000) );
      m_monitor->SetDrawOptions(h1_sumQ_wide[i],"colz");
      sprintf(temp,"Sum Q (Chan %i)",i);
      h1_sumQ_wide[i]->SetTitle(temp);

      h1_TDC.push_back( m_monitor->Book<TH1F>("h1_TDC_"+std::to_string(i),
					      "TDC Distribution (Chan "+std::to_string(i)+")",
					      ("h1_TDC_"+std::to_string(i)).data(),
					      "Events", 4,-0.5,3.5) );
      m_monitor->SetDrawOptions(h1_TDC[i],"colz");
      sprintf(temp,"TDC (Chan %i)",i);
      h1_TDC[i]->SetTitle(temp);
    }// end loop over channels

    h1_CID1 = m_monitor->Book<TH1F>("h1_CID1","Cap ID (Fiber 1)",
                                    "h1_CID1",";Cap ID;Time Samples",4,-0.5,3.5);
    m_monitor->SetDrawOptions(h1_CID1,"colz");

    h1_CID2 = m_monitor->Book<TH1F>("h1_CID2","Cap ID (Fiber 2)",
                                    "h1_CID2",";Cap ID;Time Samples",4,-0.5,3.5);
    m_monitor->SetDrawOptions(h1_CID2,"colz");

    h1_time = m_monitor->Book<TH1F>("h1_time","Trigger Time",
                                    "h1_time",";Tick;Events",200,0,10000000);
    m_monitor->SetDrawOptions(h1_time,"colz");

    h1_mult = m_monitor->Book<TH1F>("h1_mult","Hit Multiplicity",
                                    "h1_mult",";Num. Hits;Events",10,-0.5,9.5);
    m_monitor->SetDrawOptions(h1_mult,"colz");

    h1_hit_dist = m_monitor->Book<TH1F>("h1_hit_dist","Hit Distribution",
                                    "h1_hit_dist",";Channel;Hits",12,-0.5,11.5);
    m_monitor->SetDrawOptions(h1_hit_dist,"colz");

    h2_event_sum = m_monitor->Book<TH2F>("h2_event_sum","Event Summary",
                                         "h2_event_sum",";Channel;Sum Q [fC]",12,-0.5,11.5,200,0,50000);
    m_monitor->SetDrawOptions(h2_event_sum,"colz");    
    
  }// end TrigScintTestBeamMonitor::AtConfiguration

  void TrigScintTestBeamMonitor::AtEventReception(EventSP event) {
    /**
     * Drop code to extract samples here.
     */

    //0x2 is correct?
    //The getBlock will return a buffer of bytes
    std::vector<uint8_t> buffer = event->GetBlock(0x2);
    // buffers for individual words for each data fiber
    // 16 bit buffers are convenient for removing pad
    // words from the data stream
    uint16_t fiber1a,fiber2a,fiber1b,fiber2b;
    //std::cout<<"Received event, buffer size::" << buffer.size()<<std::endl;

    for (unsigned int i=0; i < buffer.size();i+=8 ) {

      fiber1a = buffer[i]   << 8 | buffer[i+1];
      fiber1b = buffer[i+2] << 8 | buffer[i+3];
      fiber2a = buffer[i+4] << 8 | buffer[i+5];
      fiber2b = buffer[i+6] << 8 | buffer[i+7];
    
      //std::cout<<std::setw(4)<<std::setfill('0')<<std::hex<<fiber1a<<" "<< std::setw(4)<<std::setfill('0')<<std::hex << fiber2a<<" "<< std::setw(4)<<std::setfill('0')<<std::hex << fiber1b<<" "<<std::setw(4)<<std::setfill('0')<<std::hex << fiber2b<<std::dec<<std::endl;
    
      // remove all comma chraacters before filling buffer
    
      // Slow    
      if( fiber1a != comma_char)
	event_buffer1.push_back(fiber1a);

      if( fiber2a != comma_char )
	event_buffer2.push_back(fiber2a);

      if( fiber1b != comma_char )
	event_buffer1.push_back(fiber1b);

      if( fiber2b != comma_char )
	event_buffer2.push_back(fiber2b);

    }

    //std::cout << "without CC (" << std::setw(4)<<std::setfill('0') << comma_char << ") " << std::endl;
	  
    //for( int i = 0 ; i < event_buffer1.size() ; i++){
    //  std::cout << std::setw(4)<<std::setfill('0') << std::hex << event_buffer1[i] <<  " " << std::setw(4)<<std::setfill('0') << std::hex << event_buffer2[i] << std::endl;
    //}

    if (event_buffer1.size() != event_buffer2.size()){
      std::cout<<"Error in decoding. Event buffers have different length"<<std::endl;
      printf("event_buffer1.size() = %li\tevent_buffer1.size() = %li\n",event_buffer1.size(),event_buffer2.size());
    }//else
    //printf("Good event. event_buffer1.size() = %li\tevent_buffer1.size() = %li\n",event_buffer1.size(),event_buffer2.size());
  
    //std::cout<<"L109: About to initialize tse\n";
    TSevent* tse = new TSevent(event_buffer1,event_buffer2); //std::cout<<"L110 Just initialized tse\n";
    // for(int i=0;i<event_buffer1.size();i++)
    //   printf("event_buffer1[%i]\t%i",i,event_buffer1[i]);
    // std::cout<<std::endl;
  
    // printf("tse->qie1_.size() %li\n",tse->qie1_.size());
    // for(int i=0;i<tse->qie1_.size();i++){
    //   for(int j=0;j<tse->qie1_[i].adc.size();j++)
    // 	printf("qie1_[%i].adc[%i] = %i",i,j,tse->qie1_[i].adc[j]);
    //   std::cout<<std::endl;
    // }
    
    if( debug ){
      tse->qie1_[0].print();
      tse->qie2_[0].print();
    }

    int num_hits=0;
    std::vector<float> sum_charge={0,0,0,0,
                                  0,0,0,0,
                                  0,0,0,0,
                                  0,0,0,0};
    for(int chan=0; chan < 8;chan++){
      for(int ts = 0; ts < tse->qie1_.size();ts++){
    	h2_ADCvT[chan]->Fill(ts,tse->qie1_[ts].adc[chan]);
        h2_QvT[chan]->Fill(ts,qie_.ADC2Q(tse->qie1_[ts].adc[chan]));
    	h2_TDCvT[chan]->Fill(ts,tse->qie1_[ts].tdc[chan]);
    	h1_ADC[chan]->Fill(tse->qie1_[ts].adc[chan]);
        h1_sumQ[chan]->Fill(qie_.ADC2Q(tse->qie1_[ts].adc[chan]));
        h1_sumQ_wide[chan]->Fill(qie_.ADC2Q(tse->qie1_[ts].adc[chan]));
    	h1_TDC[chan]->Fill(tse->qie1_[ts].tdc[chan]);
        h1_CID1->Fill(tse->qie1_[ts].cid);
        sum_charge[chan]+=qie_.ADC2Q(tse->qie1_[ts].adc[chan]);
      }// end loop over fiber 1 time samples
      for(int ts = 0; ts < tse->qie2_.size();ts++){
      	h2_ADCvT[chan+8]->Fill(ts,tse->qie2_[ts].adc[chan]);
        h2_QvT[chan+8]->Fill(ts,qie_.ADC2Q(tse->qie2_[ts].adc[chan]));
      	h2_TDCvT[chan+8]->Fill(ts,tse->qie2_[ts].tdc[chan]);
      	h1_ADC[chan+8]->Fill(tse->qie2_[ts].adc[chan]);
        h1_sumQ[chan+8]->Fill(qie_.ADC2Q(tse->qie2_[ts].adc[chan]));
        h1_sumQ_wide[chan+8]->Fill(qie_.ADC2Q(tse->qie2_[ts].adc[chan]));
      	h1_TDC[chan+8]->Fill(tse->qie2_[ts].tdc[chan]);
        h1_CID2->Fill(tse->qie2_[ts].cid);
        sum_charge[chan+8]+=qie_.ADC2Q(tse->qie2_[ts].adc[chan]);
      }// end loop over fiber 2 time samples
      if( sum_charge[chan] > 20000 ){
        num_hits++;
        h1_hit_dist->Fill(chan);
      }
      if( sum_charge[chan+8] > 20000 ){
        num_hits++;
        h1_hit_dist->Fill(chan+8);
      }
      h2_event_sum->Fill(chan,sum_charge[chan]);
      h2_event_sum->Fill(chan+8,sum_charge[chan+8]);
    }// end loop over channels

    h1_time->Fill(tse->time);
    h1_mult->Fill(num_hits);
    
    //Slow
    event_buffer1.clear();
    event_buffer2.clear();
    
  }// end TrigScintTestBeamMonitor::AtEventReception

} // namespace eudaq
