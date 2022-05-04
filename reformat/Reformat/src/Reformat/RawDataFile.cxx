
#include <dlfcn.h>

#include "Reformat/RawDataFile.h"

#include "Framework/Exception/Exception.h"

namespace reformat {

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

  return registration->second(params);
}

};  // reformat
