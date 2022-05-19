
#include <dlfcn.h>

#include "Reformat/RawDataFile.h"

#include "Framework/Exception/Exception.h"

namespace reformat {

RawDataFile::RawDataFile(const framework::config::Parameters& p)
  : theLog_{framework::logging::makeLogger(p.getParameter<std::string>("name"))},
    name_{p.getParameter<std::string>("name")} {
      reformat_log(info) << "RawDataFile " << name_ << " created.";
      std::string fn{p.getParameter<std::string>("input_file","")};
      if (not fn.empty()) {
        file_reader_.open(fn);
        if (not file_reader_) {
          EXCEPTION_RAISE("NoFile","Binary file '"+fn
              +"' could not be opened.");
        }
      }
    }

const std::string& RawDataFile::name() const {
  return name_;
}

RawDataFileFactory& RawDataFileFactory::get() {
  static RawDataFileFactory instance;
  return instance;
}

void RawDataFileFactory::loadLibrary(const std::string& libname) {
  if (libraries_loaded_.find(libname) != libraries_loaded_.end()) {
    return;  // already loaded
  }

  void* handle = dlopen(libname.c_str(), RTLD_NOW);
  if (handle == nullptr) {
    EXCEPTION_RAISE("LibraryLoadFailure",
                    "Error loading library '" + libname + "':" + dlerror());
  }
  reformat_log(debug) << "Loaded " << libname;
  libraries_loaded_.insert(libname);
}

std::unique_ptr<RawDataFile> RawDataFileFactory::create(
    const std::string& class_name, const framework::config::Parameters& params) const {
  auto registration{registered_types_.find(class_name)};
  if (registration == registered_types_.end()) {
    EXCEPTION_RAISE("BuildFail",
        "Cannot create raw data type " + class_name + ".\n"
        "Did you register the new type?");
  }
  reformat_log(debug) << "Creating an instance of " << class_name;
  return registration->second(params);
}

};  // reformat
