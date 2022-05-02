#ifndef REFORMAT_RAWDATAFILE_H_
#define REFORMAT_RAWDATAFILE_H_

#include <memory>  //for unique pointer

#include "Framework/Configure/Parameters.h"
#include "Framework/Event.h"

namespace reformat {

/// forward declaration for the builder and ptr typedefs
class RawDataFile;

/// typedef for a pointer to a raw data file
typedef std::unique_ptr<RawDataFile> RawDataFilePtr;

/// typedef for the creation of a raw data file
typedef RawDataFilePtr RawDataFileBuilder(
    const framework::config::Parameters& ps);

/**
 * @class RawDataFile
 *
 * A single file of raw data.
 */
class RawDataFile {
 public:
  /**
   * Constructor of a raw data file with a filename
   */
  RawDataFile(const framework::config::Parameters&) {}

  /// virtual and default destructor
  virtual ~RawDataFile() = default;

  /**
   * Save the next event from this data file into the passed bus.
   *
   * @note The return value of next signals when this
   * raw data file is all done.
   *
   * @return true if we are done, false otherwise
   */
  virtual bool next(framework::Event& event) = 0;

  /**
   * Declare a new raw data file type
   */
  static void declare(const std::string& classname,
                      RawDataFileBuilder* builder);
};  // RawDataFile

/**
 * @class RawDataFileFactory
 * Singleton factory to create raw data file objects
 */
class RawDataFileFactory {
 public:
  // get the factory instance
  static RawDataFileFactory& getInstance();

  /**
   * Load a library.
   * @param libname The library to load.
   */
  void loadLibrary(const std::string& libname);

  /**
   * Register a type for the factory to be able to build
   */
  void registerType(const std::string& class_name, RawDataFileBuilder* builder);

  /**
   * Create a data file
   */
  RawDataFilePtr create(const std::string& class_name,
                        const framework::config::Parameters& parameters) const;

  /// Delete the copy constructor
  RawDataFileFactory(const RawDataFileFactory&) = delete;

  /// Delete the assignment operator
  void operator=(const RawDataFileFactory&) = delete;

 private:
  /// private constructor
  RawDataFileFactory() = default;

 private:
  /// The classes that can be built (and their builders)
  std::map<std::string, RawDataFileBuilder*> registered_types_;
  /// libraries that have been loaded
  std::set<std::string> libraries_loaded_;

};  // RawDataFileFactory

}  // namespace reformat

/**
 * @def DECLARE_RAW_DATA_FILE(NS, CLASS)
 * @param NS The full namespace specification for the RawDataFile
 * @param CLASS The name of the class to register
 * @brief Macro which allows the ReformatBase library to construct a file given
 * its name during configuration.
 */
#define DECLARE_RAW_DATA_FILE(NS, CLASS)                                      \
  reformat::RawDataFilePtr CLASS##_reformat_make(                             \
      const framework::config::Parameters& ps) {                              \
    return std::make_unique<NS::CLASS>(ps);                                   \
  }                                                                           \
  __attribute__((constructor(1000))) static void CLASS##_reformat_declare() { \
    reformat::RawDataFile::declare(                                           \
        std::string(#NS) + "::" + std::string(#CLASS),                        \
        &CLASS##_reformat_make);                                              \
  }

#endif  // REFORMAT_RAWDATAFILE_H_
