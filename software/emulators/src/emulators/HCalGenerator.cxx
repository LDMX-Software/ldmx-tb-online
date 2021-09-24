#include "emulators/HCalGenerator.h"

#include <iostream>

#include <bitset>
#include "rogue/interfaces/stream/Frame.h"
#include "rogue/interfaces/stream/FrameIterator.h" 

namespace emulators {

void HCalGenerator::genFrame(uint32_t size) {
  rogue::interfaces::stream::FramePtr frame;
  rogue::interfaces::stream::FrameIterator it;

  frame = reqFrame(8, true); 
  frame->setPayload(8);

  uint32_t header{format_version_};
  header |= (fpga_id_ << fpga_id_mask_);
  header |= (n_links_ << n_links_mask_);

  // TODO: fix this calc
  uint16_t len = 4 + n_links_*10;
  header |= (len << len_mask_); 
  std::cout << "len: " << len << std::endl; 
  std::cout << "[ HCalGenerator ]: Header [0:31]: " 
	    << std::bitset<32>(header) << std::endl;   

  toFrame(it, 4, &header);

  header = bunch_id_;
  header |= (0x1 << rr_mask_); 
  header |= (orbit_counter_ << orbit_counter_mask_);   

  std::cout << "[ HCalGenerator ]: Header [32:63]: " 
	    << std::bitset<32>(header) << std::endl;   

  toFrame(it, 4, &header);

  // Increment the bunch ID
  bunch_id_ += 1; 

  // Build the ROC subpackets for each link

  // Assume 10 bunches per train
  if ((bunch_id_ + 1)%10 == 0) orbit_counter_ += 1; 

  // The CRC-32 tail -- Not currently used
  uint32_t tail{0}; 
  std::cout << "[ HCalGenerator ]: Tail: " 
	    << std::bitset<32>(tail) << std::endl;   
  toFrame(it, 4, &tail); 

  sendFrame(frame);  

}
}
