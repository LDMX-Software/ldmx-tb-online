#ifndef HEXABOARD_HGCROCV2RAWDATAFILE
#define HEXABOARD_HGCROCV2RAWDATAFILE

#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <deque>
#include <fstream>
#include <iostream>

#include "Reformat/RawDataFile.h"

namespace reformat {

/**
 * @namespace hexaboard
 *
 * Interfacing with hexactrl-sw that is being used by CMS
 * in hexaboard test-stands. The "hexaboards" in CMS are being
 * used as the ECal modules in LDMX, so we have adopted use of
 * this hexactrl-sw for LDMX test stands with HGC ROCs.
 */
namespace hexaboard {

/**
 * This raw data file wraps the Boost.Serialization output
 * of the hexactrl-sw being used to test the HGC ROC board
 * and its connection to a hexa-board in CMS.
 */
class HGCROCv2RawDataFile : public reformat::RawDataFile {
 public:
  /// open up boost binary archive from input file
  HGCROCv2RawDataFile(const framework::config::Parameters& ps)
      : input_stream_{ps.getParameter<std::string>("input_file")},
        input_archive_{input_stream_},
        reformat::RawDataFile(ps) {
    buffer_name_ = ps.getParameter<std::string>("name");
  }

  /// default destructor, closes up boost archive and input stream
  ~HGCROCv2RawDataFile() = default;

  /// read in the data for the next event and put it on the bus
  bool next(framework::Event& event);

 private:
  /// the input file stream
  std::ifstream input_stream_;
  /// the archive we are reading in
  boost::archive::binary_iarchive input_archive_;
  /// the name of the output event object
  std::string buffer_name_;

 private:
  /**
   * This class is copied almost exactly from
   * the hexactrl-sw borrowed from CMS. I have deleted the parts
   * of it that are used to write the raw data coming from the HGC ROC
   * in-order-to cleanup the code.
   *
   * Each instance of this class represents an individual sample
   * from each channel on both halves of the ROC.
   */
  class Sample {
   public:
    /// get the current event number
    int event() const { return m_event; }

    /// get the chip id number
    int chip() const { return m_chip; }

    /**
     * Put this sample into the passed buffer
     *
     * This is where we actually do the encoding.
     * We add the necessary headers and perform the CRC
     * checksums that are projected to be used.
     */
    void put(std::vector<uint32_t>& buffer) const;

    /**
     * Stream the sample class to an output stream
     *
     * Helpful for debugging
     */
    void stream(std::ostream& out) const;

    /// output stream operator
    friend std::ostream& operator<<(std::ostream& out,
                                    const HGCROCv2RawDataFile::Sample& h) {
      h.stream(out);
      return out;
    }

   private:
    /**
     * The serialization function that is used by Boost to do I/O
     */
    friend class boost::serialization::access;
    template <class Archive>
    void serialize(Archive& ar, const unsigned int version) {
      ar& m_event;
      ar& m_chip;
      ar& m_data0;
      ar& m_data1;
    }

   private:
    int m_event{-1};
    int m_chip;
    std::vector<uint32_t> m_data0;
    std::vector<uint32_t> m_data1;
  } the_sample_;  // Sample

};  // HGCROCv2RawDataFile

}  // namespace hexaboard
}  // namespace reformat

#endif
