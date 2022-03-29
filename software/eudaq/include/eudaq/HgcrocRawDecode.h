#ifndef EUDAQ_HGCROCRAWDECODE_H_
#define EUDAQ_HGCROCRAWDECODE_H_

#include <map>
#include <vector>
#include <ostream>

namespace hcal {

/**
 * The HGC ROC reads out 32-bit word "samples" which 
 * are encoded with the readout mode (ADC or TOT) and
 * the readout measurements.
 *
 * This class is almost exactly copied from the
 * HgcrocDigiCollection::Sample class within ldmx-sw.
 *
 * Not all of these measurements are valid in each sample.
 * The valid measurements depend on the tot_progress and tot_complete flags.
 *
 * The TOA measurement is always valid and is inserted as the third
 * measurement in the 32-bit word.
 *
 * If the TOT measurment is NOT complete, then the other
 * two valid measurements (in order) are
 *  1. ADC of the previous sample (adc_tm1)
 *  2. ADC of this sample (adc_t)
 *
 * If the TOT is NOT in progress and the TOT is complete, then
 *  1. ADC of the previous sample (adc_tm1)
 *  2. TOT measurement (tot)
 *
 * If both flags are true, then
 *  1. ADC of this sample (adc_t)
 *  2. TOT measurement (tot)
 *
 * Usually several samples are used for each channel to re-construct the hit.
 */
class HgcrocSample { 
 public:
  /**
   * Basic constructor
   *
   * Use this constructor when translating binary data coming off the
   * detector into our event model.
   *
   * Default is version 2 of the HGC ROC since that is what is used
   * for test beam.
   */
  HgcrocSample(uint32_t w, int version = 2);

  /**
   * Get the first flag from the sample
   * checking if TOT is in progress during this sample
   *
   * @return true if TOT is in progress during this sample
   */
  bool isTOTinProgress() const;

  /**
   * Get the second flag from the sample
   * checking if TOT is complete at this sample
   *
   * @return true if TOT is complete and should use this sample to get TOT
   * measurement
   */
  bool isTOTComplete() const;

  /**
   * Get the Time Of Arrival of this sample
   * which is always the third position in all readout modes.
   *
   * @return 10-bit measurement of TOA
   */
  int toa() const;

  /**
   * Get the TOT measurement from this sample
   *
   * @note Does not check if this is the TOT Complete sample!
   *
   * Expands the 10-bit measurment inside the sample into
   * the 12-bit actual measurement of TOT.
   *
   * @return 12-bit measurement of TOT
   */
  int tot() const;

  /**
   * Get the last ADC measurement from this sample
   *
   * @note Does not check if this sample has a valid ADC t-1 measurement.
   *
   * @return 10-bit measurement of ADC t-1
   */
  int adc_tm1() const;

  /**
   * Get the ADC measurement from this sample
   *
   * Checks which running mode we are in to determine
   * which position the measurement should be taken from.
   *
   * @return 10-bit measurement of current ADC
   */
  int adc_t() const;

  /**
   * Get the raw value of this sample
   *
   * @return 32-bit full value fo the sample
   */
  uint32_t raw() const;

 private:
  /**
   * Get the first 10-bit measurement out of the sample
   *
   * @return 10-bit measurement at first position in sample
   */
  int first() const;

  /**
   * Get the second 10-bit measurement out of the sample
   *
   * @return 10-bit measurement at second position in sample
   */
  int secon() const;

  /**
   * Get the third 10-bit measurement out of the smaple
   * @return 10-bit measurement at second position in sample
   */
  int third() const;

 private:
  /// The actual 32-bit word spit out by the chip
  uint32_t word_;
  /// version to use for {de,en}coding
  int version_;
};  // Sample

/**
 * Wrapper for holding the location of a channel using
 * electronics information.
 */
class ElectronicsLocation {
  /// polarfire fpga id
  unsigned int fpga_;
  /// link index in that fpga
  unsigned int link_;
  /// channel number on that link
  unsigned int inlink_channel_;
  /// roc index in the fpga
  unsigned int roc_;
  /// channel number on that roc
  unsigned int channel_;
 public:
  /**
   * Constructor from decoded data
   */
  ElectronicsLocation(unsigned int f, unsigned int l, unsigned int c);
  unsigned int fpga() const;
  unsigned int link() const;
  unsigned int inlink_channel() const;
  unsigned int roc() const;
  unsigned int channel() const;
  friend std::ostream& operator<<(std::ostream& os, const ElectronicsLocation& el);
  friend bool operator<(const ElectronicsLocation& lhs, const ElectronicsLocation& rhs);
};

/**
 * Decode the passed byte buffer into a map of locations to the samples taken within that channel
 *
 * Use like
 *
 * ```cpp
 * auto data = hcal::decode(event.GetBlock(1));
 * for (const auto& [el, samples] : data) {
 *   // el is hcal::ElectronicsLocation for this readout channel
 *   // samples is std::vector<hcal::HgcrocSample> for this readout channel
 * }
 * ```
 */
std::map<ElectronicsLocation, std::vector<HgcrocSample>>
decode(const std::vector<uint8_t>& binary_data);

} // namespace hcal

#endif // EUDAQ_HGCROCRAWDECODE_H
