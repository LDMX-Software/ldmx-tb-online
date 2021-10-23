#ifndef EUDAQ_HGCROCDATAPACKET_H
#define EUDAQ_HGCROCDATAPACKET_H

#include "eudaq/Event.hh" 

namespace eudaq {

struct RocSubpacket { 
  uint16_t roc_id;
  uint8_t wadd;
  uint16_t bunch_id;
  std::vector<uint16_t> adc;  
  //std::vector<short> adc(40, 0x0); 
  //std::vector<short> adct(40, 0x0); 
};

struct Sample { 
  /// Number of subpackets from the ROCs (NLINKS)
  uint16_t n_links{0x0};
  
  /// FPGA ID
  uint16_t fpga_id{0x0};
  
  /// Format version
  uint8_t format_version{0x0};
  
  /// Orbit counter
  uint16_t orbit_counter{0x0};

  /// Readout request
  uint16_t rr_count{0x0}; 

  /// The bunch ID
  uint16_t bunch_id{0x0};

  std::vector<RocSubpacket> subpackets; 
};

class HgcrocDataPacket {
public:
  /**
   * Constructor
   */
  HgcrocDataPacket(const eudaq::Event& event);

  /**
   */
  void createSample(int start, int length, std::vector<uint8_t> data); 

  /**
   */
  void createSamples(std::vector<uint8_t> data); 

  /**
   */
  RocSubpacket createRocSubpacket(int start, int lenght, std::vector<uint8_t> data); 

  std::vector<Sample> getSamples() { return samples_; }
private:
  /// 
  std::vector<Sample> samples_; 

  /// Size of header in bytes
  uint16_t header_size_{8}; 
  
  /// Size of tail
  uint16_t tail_size_{4}; 

  /// The number of samples in the event
  uint16_t n_samples_{0x0}; 

  /// FPGA ID
  uint16_t fpga_id_{0x0};

  /// Format version
  uint8_t format_version_{0x0};
  
};
} // namespace eudaq
#endif // EUDAQ_HGCROCDATAPACKET_H
