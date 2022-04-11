#ifndef EUDAQ_TRIGSCINTRAWDECODE_H
#define EUDAQ_TRIGSCINTRAWDECODE_H
#include <vector>
#include <map>
#include <iostream>
#include <fstream>

bool debug=false;
const int new_event_mark=0xFFFF;
const int comma_char=0xFBF7;

// class for managing QIE "frame"
// in which data for 8 channels and
// one time sample are packed.
class QIE{
public:

  std::vector<int> adc{0,0,0,0,0,0,0,0};
  std::vector<int> tdc{0,0,0,0,0,0,0,0};
  int reserve=0, cid=0, cide=0, bc0=0;
  QIE(){}
  void add_data(std::vector<uint8_t> data);
  void print();    
};


// class for organizing event
// data and managing the low-level
// unpacking of data
class TSevent{
public:

  // and event is two vectors for QIE frames
  // qie1_ : frames for fiber 1 and 0<=elecID_<=7
  // qie2_ : frames for fiber 1 and 8<=elecID_<=15
  //
  // each element represents a different time samples of an event
  // there are typically 32 times samples, but data corruptions
  // can cause less
  std::vector<QIE> qie1_,qie2_;
  uint64_t time=0; 
  TSevent(std::vector<uint16_t> fiber1,
	  std::vector<uint16_t> fiber2);

  // function for standardizing the comma characters
  // the represent the start of a frame
  bool start_of_time_sample(uint16_t word);

  // helper function for debugging
  void print();
  
};
#endif	// EUDAQ_TRIGSCINTRAWDECODE_H
