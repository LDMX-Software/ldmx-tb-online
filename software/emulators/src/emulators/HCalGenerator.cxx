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


  // TODO: fix this calc
  uint16_t len = 4 + n_links_*10;
  uint32_t header{len}; 
  header |= (n_links_ << n_links_mask_);
  header |= (fpga_id_ << fpga_id_mask_);
  header |= (format_version_ << format_version_mask_);

  std::cout << "len: " << len << std::endl; 
  std::cout << "[ HCalGenerator ]: Header [31:0]: " 
	        << std::bitset<32>(header) << std::endl;   

  toFrame(it, 4, &header);

  header = orbit_counter_; 
  header |= (0x1 << rr_mask_); 
  header |= (bunch_id_ << bunch_id_mask_);   

  std::cout << "[ HCalGenerator ]: Header [63:32]: " 
	        << std::bitset<32>(header) << std::endl;   

  toFrame(it, 4, &header);

  // Increment the bunch ID
  bunch_id_ += 1; 

  // Assume 10 bunches per train
  if ((bunch_id_ + 1)%10 == 0) orbit_counter_ += 1; 

  // Build the ROC subpackets for each link
  std::vector<uint32_t> subpackets{buildRocSubpackets(int(n_links_))}; 
  for (auto &subpacket : subpackets) {
    toFrame(it, 4, &subpacket); 
  }

  // The CRC-32 tail -- Not currently used
  uint32_t tail{0}; 
  std::cout << "[ HCalGenerator ]: Tail: " 
	        << std::bitset<32>(tail) << std::endl;   
  toFrame(it, 4, &tail); 

  sendFrame(frame);  

}

std::vector<uint32_t> HCalGenerator::buildRocSubpackets(int n_links) {
  std::vector<uint32_t> packet{int(n_links*42), 0x0};

  return packet;  
}

}
