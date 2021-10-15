#ifndef EMULATORS_QIEDATAPACKET_H
#define EMULATORS_QIEDATAPACKET_H

//---< StdLib >---//
#include <cinttypes>
#include <ostream>

namespace emulators {
class QieDataPacket {
public:
  /**
   * Constructor
   */
  QieDataPacket(uint8_t trig_id);

  /**
   */
  void addQieData(int qie_id, int ts, uint8_t adc, uint8_t tdc);

  /**
   */
  void setFlags(bool cid_skipped, bool cid_unsync, bool crc0_malformed,
                bool crc1_malformed);

  /**
   */
  uint8_t *formPacket();

  /**
   * Set the ADC value
   */
  void setADC(int ts, int qie, uint8_t value) { adc_[ts][qie] = value; }

  /**
   * Set the TDC value
   */
  void setTDC(int ts, int qie, uint8_t value) { tdc_[ts][qie] = value; }

  /**
   * Stream the QIE data.
   *
   * @param[in] o std::ostream to write to.
   * @param[in] data The data to write out.
   * @returns A modified ostream.
   */
  friend std::ostream &operator<<(std::ostream &o, const uint8_t* data);

private:
  /// Number of time samples considered
  static const int n_ts_{5};

  /// Number of QIE's considered.
  static const int n_qie_{12};

  /// Collection of 8-bit ADCs
  uint8_t adc_[n_ts_][n_qie_]{0};

  /// Collection of 8-bit TDCs
  uint8_t tdc_[n_ts_][n_qie_]{0};

  /// Trigger ID
  uint16_t trig_id_;

  /// If Cap. ID got skipped
  bool cid_skipped_{false};

  /// If Cap. ID was out of sync
  bool cid_unsync_{false};

  ///
  bool crc0_malformed_{false};

  ///
  bool crc1_malformed_{false};
};
} // namespace emulators
#endif // EMULATORS_QIEDATAPACKET_H
