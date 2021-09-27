#ifndef EMULATORS_HCALGENERATOR_H
#define EMULATORS_HCALGENERATOR_H

#include "emulators/Generator.h"

namespace emulators {

class HCalGenerator : public Generator {

public:
  static std::shared_ptr<HCalGenerator> create() {
    static std::shared_ptr<HCalGenerator> ret = std::make_shared<HCalGenerator>();
    return (ret);
  }

  /// Constructor
  HCalGenerator() : Generator() {};

  /// Default Constructor
  ~HCalGenerator() = default;

  /**
   * Generate a data frame of the give size.
   *
   * @param size The size of the frame in bytes.
   */
  void genFrame(uint32_t size);

  /** @return the TX count */
  uint32_t getTxCount() {};

  /** @return the total TX bytes */
  uint32_t getTxBytes() {};

  /** @return the total TX errors */
  uint32_t getTxErrors() {};

private:
  /**
   */
  std::vector<uint32_t> buildRocSubpackets(int n_links); 


  /// Number of subpackets from the ROCs (NLINKS)
  uint8_t n_links_{0x1};

 /// NLINKS mask
 uint8_t n_links_mask_{0xE};

  /// FPGA ID
  uint8_t fpga_id_{0x2}; 

  /// FPGA ID Mask
  uint8_t fpga_id_mask_{0x14};

  /// Format version
  uint8_t format_version_{0x1};

  /// Format version mask
  uint8_t format_version_mask_{0x1C}; 

 /// Orbit counter
 uint16_t orbit_counter_{0}; 

 /// Readout request mask
 uint8_t rr_mask_{0xA}; 

 /// The bunch ID
 uint16_t bunch_id_{0}; 

 /// Orbit counter mask
 uint8_t bunch_id_mask_{0x14}; 

};

} // namespace emulators

#endif // EMULATORS_HCALGENERATOR_H
