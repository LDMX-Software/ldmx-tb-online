#include "eudaq/TrigScintRawDecode.h"


    // method for retreive raw data for an
    // individual frame and unpacking it into the
    // data members of this class
void QIE::add_data(std::vector<uint8_t> data){
      
      if ( data.size() != 12 ){
	std::cout << "OOPS! received " << data.size() << "/12 bytes" << std::endl;
	return;
      }
    
      bc0     =  (data[1]&1);
      cide    = ((data[1]>>1)&1);
      cid     = ((data[1]>>2)&3);
      reserve = ((data[1]>>4)&7);
    
      // stitch together TDC words
      uint16_t TDCs = (data[10]<<8)&data[11];
      //printf("TrigScintRawDecode.cxx L20 TDCs %i\n",TDCs);
    
      // extract ADC and TDC values
      for ( int q = 0 ; q < 8 ; q++ ){
	adc.push_back(data[q+2]);
	tdc.push_back(( TDCs>>(q*2) )&3);
      }
  
    }// end QIE::add_data

    // helper function for debugging
void QIE::print(){
      std::cout << "[[QIE]]" << std::endl;
      std::cout << "reserve: "  << reserve << std::endl;
      std::cout << "Cap ID: " << cid << std::endl;
      std::cout << "error: " << cide << std::endl;
      std::cout << "BC0: " << bc0 << std::endl;
      for ( int i = 0 ; i < adc.size() ; i ++ ) {
	std::cout << " ADC " << i << " : " << adc[i] ;
      
      }
      std::cout << std::endl;
      for ( int i = 0 ; i < tdc.size() ; i ++ ) {
	std::cout << " TDC " << i << " : " << tdc[i];
      }
      std::cout << std::endl;
    }// end QIE::print

  
TSevent::TSevent(std::vector<uint16_t> fiber1,
	  std::vector<uint16_t> fiber2){

  if(debug) printf("TrigScintRawDecode.cxx L52 fiber1.size() = %li\tfiber2.size() = %li\n",fiber1.size(),fiber2.size());
    // extract time since the start of spill from
    // the event
    time|=uint64_t(fiber2[1]);
    time|=uint64_t(fiber1[1])<<16;
    time|=uint64_t(fiber2[0])<<32;
    time|=uint64_t(fiber1[0])<<48;
    if ( debug ) std::cout << "time: " << std::hex << time << std::endl;
    
    // strip frames until the first bc
    // skipping first two words
    //
    // The data format is most easily decoded with 8 bit words.
    // So, the 16 bit words are split before being passed to
    // the QIE unpacker.
    std::vector<uint8_t> buffer;
    std::vector<uint8_t> res_buffer;
    // a flag used to keep track of partial frames that can
    // occur at the beginning of an event.
    bool first = true;
    int iword = 0;

    for (auto word : fiber1 ){
      if( debug ){
	std::cout << "buffer size: " << buffer.size() << std::endl;
	std::cout << "word: " << std::hex << word << std::endl;
	std::cout << "first byte: " << std::hex << (word&0xFF) << std::endl;
      }
      //std::cout<<"TrigScintRawDecode.cxx L79 first "<<first<<" word "<<word<<" start_of_time_sample(word) "<<start_of_time_sample(word)<<"\n";      
      
      //skip first two words -- already handled above with the time extraction
      if( iword < 2 ){
	if( debug ) std::cout << "skip first two words" << std::endl;
	iword++;
	continue;
      }
      
      //skip words between 2 and first 0xBC (or 0xFC which occurs every 3564 time samples)
      if( first && !start_of_time_sample(word) ){
	if( debug ) std::cout << "skip words until first 0xBC" << std::endl;
	res_buffer.push_back(word&0xF);
	res_buffer.push_back((word>>8)&0xF);
	iword++;
	continue;
      }

      //for each 0xBC (or 0xFC) form new QIE object and clear buffer
      if( !first && start_of_time_sample(word) ){
	if( debug ) std::cout << "new time sample" << std::endl;
	QIE temp_qie;
	temp_qie.add_data(buffer);
	//temp_qie.print();
	qie1_.push_back(temp_qie);
	buffer.clear();
      }

      //mark occurence of first 0xBC (or 0xFC)
      if( first && start_of_time_sample(word) ){
	if( debug ) std::cout << "first fournd" << std::endl;
	first =false;
      }
      buffer.push_back(word&0xF);
      buffer.push_back((word>>8)&0xF);
      iword++;
    }

    // it looks like the extra words at the begging and
    // end of the event exactly form one extra frame,
    // which could be due to a pointer misalignment in a
    // circular buffer in the f/w???
    if( buffer.size() + res_buffer.size() == 12 ){
      for( auto word : res_buffer ){
	buffer.push_back(word);
      }
      QIE temp_qie;
      temp_qie.add_data(buffer);
      qie1_.push_back(temp_qie);
    }
    buffer.clear();
    res_buffer.clear();
    
    // repeat for data on fiber 2
    for (auto word : fiber2 ){
      if( debug ){
	std::cout << "buffer size: " << buffer.size() << std::endl;
	std::cout << "word: " << std::hex << word << std::endl;
	std::cout << "first byte: " << std::hex << (word&0xFF) << std::endl;
      }

      //skip first two words -- already handled above with the time extraction
      if( iword < 2 ){
	if( debug ) std::cout << "skip first two words" << std::endl;
	iword++;
	continue;
      }

      //skip words between 2 and first 0xBC (or 0xFC which occurs every 3564 time samples)
      if( first && !start_of_time_sample(word)){
	if( debug ) std::cout << "skip words until first 0xBC" << std::endl;
	res_buffer.push_back(word&0xF);
	res_buffer.push_back((word>>8)&0xF);
	iword++;
	continue;
      }
      //for each 0xBC (or 0xFC) form new QIE object and clear buffer
      if( !first && start_of_time_sample(word)){
	if( debug ) std::cout << "new time sample" << std::endl;
	QIE temp_qie;
	temp_qie.add_data(buffer);
	//temp_qie.print();
	qie2_.push_back(temp_qie);
	buffer.clear();
      }
      //mark occurence of first 0xBC (or 0xFC)
      if( first && start_of_time_sample(word)){
	if( debug ) std::cout << "first fournd" << std::endl;
	first =false;
      }
      buffer.push_back(word&0xF);
      buffer.push_back((word>>8)&0xF);
      iword++;
    }

    if( buffer.size() + res_buffer.size() == 12 ){
      for( auto word : res_buffer ){
	buffer.push_back(word);
      }
      QIE temp_qie;
      temp_qie.add_data(buffer);
      qie2_.push_back(temp_qie);
    }
    buffer.clear();
    res_buffer.clear();

    // in case there is a mismatch, remove
    // frames until fiber1 and fiber2 data
    // matches.  There should be 32 frames for
    // each, though.
    while( qie1_.size() > qie2_.size() ){
      qie1_.erase(qie1_.begin());
    }
    while( qie2_.size() > qie1_.size() ){
      qie2_.erase(qie2_.begin());
    }
    
  }// end of TSevent::TSevent

bool TSevent::start_of_time_sample(uint16_t word){
    return ( (word&0xFF) == 0xBC ) || ( (word&0xFF) == 0xFC );
  }// end TSevent::start_of_time_sample

void TSevent::print(){
    std::cout << "[[TSevent]]" << std::endl;
    std::cout << "Time: " << std::hex << time << std::endl;
    std::cout << "FIBER1 (" << qie1_.size() << " time samples)" << std::endl;
    for( auto q : qie1_ ){
      q.print();
    }
    std::cout << "FIBER2 (" << qie2_.size() << " time samples)" << std::endl;
    for( auto q : qie2_ ){
      q.print();
    }
}// end TSevent::print
