#ifndef EMULATORS_QIE_DATAPACKET_H
#define EMULATORS_QIE_DATAPACKET_H
#include <cinttypes>
#include <vector>

namespace emulators {
  class QIE_DataPacket
  {
  public:
    QIE_DataPacket(uint8_t trig_id_);
    void AddQIEData(int qie_id,int ts,uint8_t adc_, uint8_t tdc_);
    void SetFlags(bool CID_skipped_,bool CID_unsync_,
		  bool CRC0_malformed_,bool CRC1_malformed_);
    uint8_t* FormPacket();
    void PrintQIEInfo(uint8_t* data);
    
    /// no. of time samples considered
    static const int n_ts{5};

    /// no. of qie's considered.
    static const int n_qie{12};

    /// Collection of 8-bit ADCs
    uint8_t adc[n_ts][n_qie]{0};

    /// Collection of 8-bit TDCs
    uint8_t tdc[n_ts][n_qie]{0};

  private:
    /// Trigger ID
    uint16_t trig_id;

    /// If Cap. ID got skipped
    bool CID_skipped{false};

    /// If Cap. ID was out of sync
    bool CID_unsync{false};
    bool CRC0_malformed{false};
    bool CRC1_malformed{false};
  };
} // namespace emulators
#endif // EMULATORS_QIE_DATAPACKET_H
