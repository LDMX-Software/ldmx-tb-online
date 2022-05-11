#ifndef REFORMAT_RAWDATAFILE_H_
#define REFORMAT_RAWDATAFILE_H_

#include <memory>  //for unique pointer
#include <functional>

#include "Framework/Configure/Parameters.h"
#include "Framework/Event.h"
#include "Framework/Logger.h"

#include "Reformat/EventPacket.h"
#include "Reformat/Utility/Reader.h"

namespace reformat {

/**
 * @class RawDataFile
 *
 * A single file of raw data.
 */
class RawDataFile {
 public:
  /**
   * Constructor of a raw data file with a filename
   *
   * Channel name for logger taken from 'name' parameter
   * in Parameters.
   */
  RawDataFile(const framework::config::Parameters& p);

  /// virtual and default destructor
  virtual ~RawDataFile() = default;

  /**
   * The name that should be used for this event object
   */
  virtual const std::string& name() const;

  /**
   * Save the next event from this data file into the passed bus.
   *
   * @note The return value of next signals when this
   * raw data file is all done. Return the empty braces `{}`
   * to signal that the file is all done.
   *
   * @return true if we are done, false otherwise
   */
  virtual std::optional<EventPacket> next() = 0;
 
 protected:
  /// the logger for this raw data file
  mutable framework::logging::logger theLog_;

  /// the binary file reader
  reformat::utility::Reader file_reader_;

 private:
  /// the name given to this raw data file
  std::string name_;
};  // RawDataFile

/**
 * @class RawDataFileFactory
 * Singleton factory to create raw data file objects
 */
class RawDataFileFactory {
 public:
  // get the factory instance
  static RawDataFileFactory& get();

  /**
   * Load a library.
   * @param libname The library to load.
   */
  void loadLibrary(const std::string& libname);

  /**
   * Register a type for the factory to be able to build
   */
  template<typename DerivedType>
  uint64_t declare(const std::string& name) {
    registered_types_[name] = &maker<DerivedType>;
    return reinterpret_cast<uint64_t>(&registered_types_);
  }

  /**
   * Create a data file
   */
  std::unique_ptr<RawDataFile> create(const std::string& class_name, 
      const framework::config::Parameters& parameters) const;

  /// Delete the copy constructor
  RawDataFileFactory(const RawDataFileFactory&) = delete;

  /// Delete the assignment operator
  void operator=(const RawDataFileFactory&) = delete;

 private:
  /// private constructor
  RawDataFileFactory() = default;

  template<typename DerivedType>
  static std::unique_ptr<RawDataFile> maker(const framework::config::Parameters& p) {
    return std::make_unique<DerivedType>(p);
  }

 private:
  /// The classes that can be built (and their builders)
  std::map<std::string, 
    std::function<std::unique_ptr<RawDataFile>(
        const framework::config::Parameters&)>> registered_types_;
  /// libraries that have been loaded
  std::set<std::string> libraries_loaded_;

  /// Enable logging for raw data file factory
  mutable framework::logging::logger theLog_{framework::logging::makeLogger("RawDataFileFactory")};
};  // RawDataFileFactory

}  // namespace reformat

/**
 * @def DECLARE_RAW_DATA_FILE(CLASS)
 * @param CLASS The full name of the class to register
 * @brief Macro which allows the ReformatBase library to construct a file given
 * its name during configuration.
 */
#define DECLARE_RAW_DATA_FILE(CLASS)                                     \
  namespace {                                                            \
    auto v = reformat::RawDataFileFactory::get().declare<CLASS>(#CLASS); \
  }

/**
 * Log a message at the input logging level
 */
#define reformat_log(lvl) ldmx_log(lvl)

#endif  // REFORMAT_RAWDATAFILE_H_
