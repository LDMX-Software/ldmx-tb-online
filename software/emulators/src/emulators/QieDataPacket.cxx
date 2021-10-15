#include "emulators/QieDataPacket.h"

#include <iomanip>

namespace emulators {
QieDataPacket::QieDataPacket(uint8_t trig_id) { trig_id_ = trig_id; }

void QieDataPacket::addQieData(int qie_id, int ts, uint8_t adc, uint8_t tdc) {
  adc_[ts][qie_id] = adc;
  tdc_[ts][qie_id] = tdc;
}

void QieDataPacket::setFlags(bool cid_skipped, bool cid_unsync,
                             bool crc0_malformed, bool crc1_malformed) {

  cid_skipped_ = cid_skipped;
  cid_unsync_ = cid_unsync;
  crc0_malformed_ = crc0_malformed;
  crc1_malformed_ = crc1_malformed;
}

uint8_t *QieDataPacket::formPacket() {

  auto nwords{4 + 2 * n_ts_ * n_qie_};
  auto n_adc_words{n_qie_ / 4};
  auto data{new uint8_t[nwords]};

  // Header word
  data[0] = trig_id_ >> 8;
  data[1] = trig_id_ & 255;
  data[2] =
      crc1_malformed_ + crc0_malformed_ * 2 + cid_unsync_ * 4 + cid_skipped_ * 8;
  data[3] = 0; // Checksum.

  // Actual data
  // Counter for data[] id in use
  auto data_id{4};
  for (int ts{0}; ts < n_ts_; ++ts) {

    for (int qie{0}; qie < n_qie_; ++qie) {
      data[data_id] = adc_[ts][qie];
      ++data_id;
    }

    for (int qie{0}; qie < n_qie_; ++qie) {
      data[data_id] = tdc_[ts][qie];
      ++data_id;
    }
  }
  return data;
}

std::ostream &operator<<(std::ostream &o, const uint8_t *data) {
  int nwords{1 + QieDataPacket::n_ts_ * QieDataPacket::n_qie_ / 2};
  int n_adc_words{QieDataPacket::n_qie_ / 4};
  o << "QIE Packet {\n"
    << "\t Trigger id: " << (data[0] * 255 + data[1]) << "\n"
    << "\t---< Flags >--- (1=true, 0=false)\n"
    << "\t\tCID skipped: " << ((data[2] >> 3) & 1) << "\n"
    << "\t\tCID unsync: " << ((data[2] >> 2) & 1) << "\n"
    << "\t\tCRC0 malformed: " << ((data[2] >> 1) & 1) << "\n"
    << "\t\tCRC1 malformed: " << (data[2] & 1) << "\n"
    << "\tChecksum: " << data[3] << "\n"
    << "\t---< Data >--- " << std::endl;

  for (int i{0}; i < QieDataPacket::n_qie_; ++i)
    o << "\t\tQIE" << i << std::endl;

  int data_id = 4; // counter for data[] id in use
  for (int ts{0}; ts < QieDataPacket::n_ts_; ++ts) {
    o << "\t\tADC: [ ";

    for (int qie{0}; qie < QieDataPacket::n_qie_; ++qie) {
      o << data[data_id];
      if (qie != (QieDataPacket::n_qie_ - 1))
        o << ", ";
      data_id++;
    }
    o << " ]" << std::endl;

    o << "\t\tTDC: [ ";
    for (int qie{0}; qie < QieDataPacket::n_qie_; ++qie) {
      o << data[data_id];
      if (qie != (QieDataPacket::n_qie_ - 1))
        o << ", ";
      data_id++;
    }
    o << " ]" << std::endl;
  }
  o << "}" << std::endl;
}
} // namespace emulators
