#ifndef REFORMAT_SPILLWISECONVERTER_H_
#define REFORMAT_SPILLWISECONVERTER_H_

#include "Framework/Exception/Exception.h"
#include "Reformat/RawDataFile.h"
#include "Reformat/Converter.h"
#include "Reformat/Utility/TimestampCloud.h"

namespace reformat {

/**
 * Class to convert the passed raw data files into a
 * single Framework EventFile.
 */
class SpillwiseConverter : public Converter{
 public:
  /// un-interesting default constructor
  SpillwiseConverter() = default;
  /// close up logging that was opened in configure
  ~SpillwiseConverter();

  /**
   * Configure our converter based off the configuration parameters
   * decoded from the passed python script
   */
  void configure(const framework::config::Parameters& configuration);

  /// actual run conversion
  void convert();

 private:
  /// set of input data files
  std::vector<std::unique_ptr<RawDataFile>> input_files_;
  /// pass name to use for output event tree
  std::string pass_;
  /// output file name to write data to
  std::string output_filename_;
  /// name of detector for run header
  std::string detector_name_;
  /// run number to use for output event tree
  int run_;
  /// starting event number
  int start_event_;
  /// maximum difference between two timestamps to allow alignment
  //int max_diff_;
  /// event limit to exit early
  int event_limit_;
  /// keep all events even ones where not all raw data files participate
  bool keep_all_;
  /// frequency with which to print event status
  int print_frequency_;

  /// require white rabbit data
  bool require_WR_;

  /// log through the 'converter' channel
  mutable framework::logging::logger theLog_{framework::logging::makeLogger("Converter")};
};  // SpillwiseConverter

}  // namespace reformat

#endif  // REFORMAT_SPILLWISECONVERTER_H_
