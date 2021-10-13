#include "emulators/TrigScintReceiver.h"

//---< StdLib >---//
#include <bitset>
#include <iostream>

#include "rogue/interfaces/stream/Frame.h"
#include "rogue/interfaces/stream/FrameIterator.h"

#include "emulators/QIE_DataPacket.h"

namespace emulators {

  void TrigScintReceiver::acceptFrame(
  				      std::shared_ptr<rogue::interfaces::stream::Frame> frame) {

    // Get an iterator to the data in the frame
    auto it{frame->begin()};

    // Check for errors. If errors are found, skip processing of the frame.
    if (frame->getError()) {
      ++rx_error_count_;
      return;
    }

    //   // Extract all info from the header
    //   uint16_t word;
    //   rogue::interfaces::stream::fromFrame(it, 2, &word);

    //   std::cout << "[ TrigScintReceiver ]: FMTVER: " << getField(word, 3, 0) << std::endl;
    //   std::cout << "[ TrigScintReceiver ]: MODULE ID: " << getField(word, 5, 4) << std::endl;
    //   std::cout << "[ TrigScintReceiver ]: CARD_ID: " << getField(word, 7, 6) << std::endl;
    //   std::cout << "[ TrigScintReceiver ]: CID unsynced: " << getField(word, 8, 8) << std::endl;
    //   std::cout << "[ TrigScintReceiver ]: CID malformed: " << getField(word, 9, 9) << std::endl;
    //   std::cout << "[ TrigScintReceiver ]: CRC1 malformed: " << getField(word, 10, 10) << std::endl;
    //   std::cout << "[ TrigScintReceiver ]: QIE COUNT: " << getField(word, 15, 11) << std::endl;

    //   rogue::interfaces::stream::fromFrame(it, 2, &word);
    //   std::cout << "[ TrigScintReceiver ]: TRIG COUNT: " << word << std::endl;

    //   ++rx_count_;
    //   rx_bytes_ += frame->getPayload();

    // Extract all info from the header
    uint8_t word;
    int n_ts = 5;
    int n_qie = 12;
    
    rogue::interfaces::stream::fromFrame(it, 1, &word);
    int trig_id = 255*word;
    rogue::interfaces::stream::fromFrame(it, 1, &word);
    trig_id += word;
    QIE_DataPacket* packet = new QIE_DataPacket(trig_id);
    
    rogue::interfaces::stream::fromFrame(it, 1, &word);
    packet->SetFlags(
		  ((word>>3)&1),
		  ((word>>2)&1),
		  ((word>>1)&1),
		  (word&1));

    rogue::interfaces::stream::fromFrame(it, 1, &word);
    for(int ts=0;ts<n_ts;ts++) {
      for(int qie=0;qie<n_qie;qie++) {
	rogue::interfaces::stream::fromFrame(it, 1, &word);
	packet->adc[ts][qie]=word;
      }
      
      for(int qie=0;qie<n_qie;qie++) {
	rogue::interfaces::stream::fromFrame(it, 1, &word);
	packet->tdc[ts][qie]=word;
      }
    }

    packet->PrintQIEInfo(packet->FormPacket());
    
    ++rx_count_;
    rx_bytes_ += frame->getPayload();
  }

}; // namespace emulators
