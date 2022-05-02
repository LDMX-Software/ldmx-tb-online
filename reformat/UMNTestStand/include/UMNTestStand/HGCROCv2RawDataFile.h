#ifndef UMNTESTSTAND_HGCROCV2RAWDATAFILE
#define UMNTESTSTAND_HGCROCV2RAWDATAFILE

#include <fstream>
#include <iostream>

#include "Reformat/RawDataFile.h"
#include "Reformat/Utility/Reader.h"

namespace reformat {

/**
 * @namespace umnteststand
 */
namespace umnteststand {

/**
 * This raw data file wraps the binary data written out
 * by uMNio connected to the HGC ROC v2 on the UMN Test Stand.
 */
class HGCROCv2RawDataFile : public reformat::RawDataFile {
 public:
  /// open up boost binary archive from input file
  HGCROCv2RawDataFile(const framework::config::Parameters& ps)
      : reader_{ps.getParameter<std::string>("input_file")},
        reformat::RawDataFile(ps) {
    buffer_name_ = ps.getParameter<std::string>("name");
  }

  /// default destructor, closes up boost archive and input stream
  ~HGCROCv2RawDataFile() = default;

  /// read in the data for the next event and put it on the bus
  bool next(framework::Event& event);

 private:
  /// the input file 
  reformat::utility::Reader reader_;
  /// the name of the output event object
  std::string buffer_name_;

};  // HGCROCv2RawDataFile

}
}  // namespace reformat

#endif
