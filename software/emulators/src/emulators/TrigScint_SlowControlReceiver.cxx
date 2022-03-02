#include "emulators/TrigScint_SlowControlReceiver.h"

//---< StdLib >---//
#include <bitset>
#include <iostream>
// #include "string.h"

#include "rogue/interfaces/stream/Frame.h"
#include "rogue/interfaces/stream/FrameIterator.h"

namespace emulators {

void TrigScint_SlowControlReceiver::acceptFrame(
    std::shared_ptr<rogue::interfaces::stream::Frame> frame) {

  // Get an iterator to the data in the frame
  auto it{frame->begin()};

  // Check for errors. If errors are found, skip processing of the frame.
  if (frame->getError()) {
    ++rx_error_count_;
    return;
  }

  // Extract all info from the header
  uint16_t word;
  rogue::interfaces::stream::fromFrame(it, 2, &word);

  std::cout << "[ TrigScint_SlowControlReceiver ]: FMTVER: " << getField(word, 3, 0) << std::endl;
  std::cout << "[ TrigScint_SlowControlReceiver ]: MODULE ID: " << getField(word, 5, 4) << std::endl;
  std::cout << "[ TrigScint_SlowControlReceiver ]: CARD_ID: " << getField(word, 7, 6) << std::endl;
  std::cout << "[ TrigScint_SlowControlReceiver ]: CID unsynced: " << getField(word, 8, 8) << std::endl;
  std::cout << "[ TrigScint_SlowControlReceiver ]: CID malformed: " << getField(word, 9, 9) << std::endl;
  std::cout << "[ TrigScint_SlowControlReceiver ]: CRC1 malformed: " << getField(word, 10, 10) << std::endl;
  std::cout << "[ TrigScint_SlowControlReceiver ]: QIE COUNT: " << getField(word, 15, 11) << std::endl;

  rogue::interfaces::stream::fromFrame(it, 2, &word);
  std::cout << "[ TrigScint_SlowControlReceiver ]: TRIG COUNT: " << word << std::endl;

  ++rx_count_;
  rx_bytes_ += frame->getPayload();
}

  void TrigScint_SlowControlReceiver::PrintQIEInfo(std::vector<uint8_t> word_) {
    std::printf("Raw data:\t");
    for(auto dat : word_) std::cout<<dat<<"\t";
    
    string status1[2] = {"Off","On"};
    string status2[2] = {"low","high"};
    string ck_input[2]={"LVDS","SLVS"};
    string range_status[2] = {"auto-range","fixed range"};
    string TDC_mode[2] = {"First","Last"};
    std::printf('Input type select for input Ck receiver: %s',ck_input[word_[0]&1]); // bit 0
    std::printf('LVDS output level trim: %d',LVDS_V_OUT[(word_[0]&6)>>1]); 		  // bit 1-2
    std::printf('Discriminator state: %s',status1[(word_[0]&8)>>3]); 		  // bit 3
    std::printf('timing amplifier gain: %s',status2[(word_[0]&16)>>4]); 		  // bit 4

    ////// For the following calculations, refer to QIE11 2015 specs, pg21-22
    std::printf('DAC Timing Threshold: %d',(word_[0]>>5+word_[1]<<3));		  // bit 5-12
    std::printf('TDC LSB used: %.3f',TDC_Threshold_LSB);
    float digital_thr = word_[0]>>5+word_[1]<<3;
    float Q_slope = 23.9059;       						  // (6120-24)/(2^8-1)
    float Q_thr = 24 + Q_slope*digital_thr;
    float I_thr = Q_thr*TDC_Threshold_LSB/24;
    std::printf('TDC Threshold: %.3f uA',I_thr);

    std::printf('TimingIref: %d',(word_[1]>>5)); // bit 13-15

    float abs_pedestal = 2*(word_[2]&31);
    float pol = ((word_[2]>>5)&1);
    std::printf("pedestal DAC: %dfC",(abs_pedestal*(1-2*pol))); 		  // bit 16-21

    std::printf("capID0 pedestal DAC: %.1ffC",CapID_Pedestal((word_[3]&3)*4+word_[2]>>6));  // bit 22-25
    std::printf("capID1 pedestal DAC: %.1ffC",CapID_Pedestal((word_[3]>>2)&15)); 		// bit 26-29
    std::printf("capID2 pedestal DAC: %.1ffC",CapID_Pedestal((word_[4]&3)*4+word_[3]>>6)); 	// bit 30-33
    std::printf("capID3 pedestal DAC: %.1ffC",CapID_Pedestal((word_[4]>>2)&15)); 		// bit 34-37

    int id = (int)((word_[4]>>6)&1);
    std::printf("Digitization range mode: %s",range_status[id]); 		// bit 38
    std::printf("The digitization range set in fixed-range mode: %d",(word_[4]>>7+2*(word_[5]&1)));	// bit 39-40
    std::printf("Amount of charge injection: %dfC",90*(1+(word_[5]>>3)&7)); 			// bit 41-43
    std::printf("Transfer gain: %.1ffC/LSB",Gsel[word_[5]>>4+16*(word_[6]&1)]); 		// bit 44-48
    std::printf("input splitter bias level (Idcset): %.1fuA/LSB",4.5*((word_[6]>>1)&31)); // bit 49-53
    std::printf("LVDS CkOut: %s",status1[(word_[6]>>6)&1]); 				// bit 54
    std::printf("which pulse to consider for TDC (TDCmode): %s",TDC_mode[word_[6]>>7]); 	// bit 55
    std::printf("Hysteresis select for the timing discriminator(Hsel): %s",status2[word_[7]&1]); // bit 56
    std::printf("phasing delay betw'n input clk & internal integration window(PhaseDelay): %s",PhaseDelay(word_[7]>>1)); // bit 57-63
  }

}; // namespace emulators
