#include <iomanip>
#include "emulators/QIE_DataPacket.h"

namespace emulators {
  QIE_DataPacket::QIE_DataPacket(uint8_t trig_id_) {
    trig_id = trig_id_;
  }
  
  void QIE_DataPacket::AddQIEData(int qie_id,
		  int ts,
		  uint8_t adc_,
		  uint8_t tdc_) {
    adc[ts][qie_id] = adc_;
    tdc[ts][qie_id] = tdc_;
  }

  void QIE_DataPacket::SetFlags(bool CID_skipped_,
		bool CID_unsync_,
		bool CRC0_malformed_,
		bool CRC1_malformed_) {

    bool CID_skipped = CID_skipped_;
    bool CID_unsync = CID_unsync_;
    bool CRC0_malformed = CRC0_malformed_;
    bool CRC1_malformed = CRC1_malformed_;
  }
  
  uint32_t* QIE_DataPacket::FormPacket() {
    int nwords = 1+n_ts*n_qie/2;
    int n_adc_words = n_qie/4;
    uint32_t* data = new uint32_t[nwords];

    // Header word
    data[0] = trig_id;
    data[0] = (data[0]<<8) + trig_id;
    data[0]<<4;    // undecided bits
    data[0] = data[0] | CID_skipped;
    data[0] = (data[0]<<1) | CID_unsync;
    data[0] = (data[0]<<1) | CRC0_malformed;
    data[0] = (data[0]<<1) | CRC1_malformed;
    data[0] = data[0]<<8;	// Checksum

    // Actual data
    for(int ts=0;ts<n_ts;ts++) {
      for(int qie=0;qie<n_qie;qie++) {
	int id = 1+2*n_adc_words*ts*(int)(qie/3);
	data[id] = (data[id]<<8) | adc[ts][qie];
      }
      for(int qie=0;qie<n_qie;qie++) {
	int id = 1+n_adc_words*(1+2*ts*(int)(qie/3));
	data[id] = (data[id]<<8) | tdc[ts][qie];
      }
    }

    return data;
  }
  
  void QIE_DataPacket::PrintQIEInfo(uint32_t* data) {
    int nwords = 1+n_ts*n_qie/2;
    int n_adc_words = n_qie/4;
    printf("Header. (1=true, 0=false)\n");
    printf("trigger id: %d\n",data[0]>>24);
    printf("CID_skipped: %d\n",((data[0]>>11)&1));
    printf("CID_unsync: %d\n",((data[0]>>10)&1));
    printf("CRC0_malformed: %d\n",((data[0]>>9)&1));
    printf("CRC1_malformed: %d\n",((data[0]>>8)&1));
    printf("checksum: %d\n",(data[0]&255));

    for(int i=0;i<n_qie;i++) printf("\tQIE%2d",i);
    
    for(int ts=0;ts<n_ts;ts++) {
      printf("\nadc\t");
      for(int qie=0;qie<n_qie;qie++) {
	int id = 1+2*n_adc_words*ts*(int)(qie/3);
	printf("%3d\t",(data[id]>>(8*(qie%4))&255));
      }
      printf("\ntdc\t");
      for(int qie=0;qie<n_qie;qie++) {
	int id = 1+2*n_adc_words*ts*(int)(qie/3);
	printf("%3d\t",(data[id]>>(8*(qie%4))&255));
      }
    }
  }

} // namespace emulators
