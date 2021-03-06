
//----------------//
//   C++ StdLib   //
//----------------//
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

//-------------//
//   ldmx-sw   //
//-------------//
#include "Framework/ConfigurePython.h"

//------------//
// ReformatBase //
#include "Reformat/Converter.h"

/**
 * @func usage
 *
 * Print how to use this executable to the terminal.
 */
void usage();

/**
 * reformat
 */
int main(int argc, char* argv[]) {
  if (argc < 2) {
    usage();
    return 1;
  }

  int ptrpy = 1;
  for (ptrpy = 1; ptrpy < argc; ptrpy++) {
    if (strstr(argv[ptrpy], ".py")) break;
  }

  if (ptrpy == argc) {
    usage();
    std::cout << " ** No python configuration script provided (must end in "
                 "'.py'). ** "
              << std::endl;
    return 1;
  }

  framework::config::Parameters configuration;
  try {
    framework::ConfigurePython::root_module = "reformat";
    framework::ConfigurePython::root_class  = "Converter";
    framework::ConfigurePython::root_object = "lastConverter";

    framework::ConfigurePython cfg(argv[ptrpy], argv + ptrpy + 1,
                                   argc - ptrpy - 1);
    configuration = cfg.get();
  } catch (const framework::exception::Exception& e) {
    std::cerr << "[" << e.name() << "] : " << e.message() << std::endl;
    return 1;
  }

  framework::logging::logger theLog_{framework::logging::makeLogger("reformat")};
  reformat::Converter c;
  try {
    // logging opened here
    c.configure(configuration);
  } catch (framework::exception::Exception& e) {
    reformat_log(fatal)
      << "Config Error [" << e.name() << "] : " << e.message()
      << "\n at " << e.module() << ":" << e.line() << " in " << e.function()
      << "\nStack trace: \n" << e.stackTrace();
    return 1;
  }

  // If Ctrl-c is used, immediately exit the application.
  struct sigaction act;
  memset(&act, '\0', sizeof(act));
  if (sigaction(SIGINT, &act, NULL) < 0) {
    perror("sigaction");
    return 1;
  }

  try {
    c.convert();
  } catch (framework::exception::Exception& e) {
    reformat_log(fatal)
      << "Run Error [" << e.name() << "] : " << e.message()
      << "\n  at " << e.module() << ":" << e.line() << " in " << e.function()
      << "\nStack trace: \n" << e.stackTrace();
    return 127;  // return non-zero error-status
  } catch (std::exception& e) {
    reformat_log(fatal) << "Unrecognized Error: " << e.what();
    return 127;
  }

  // logging closed when Converter is destructed
  return 0;
}

void usage() {
  std::cout << "Usage: reformat {configuration_script.py} [arguments to "
               "configuration script]"
            << std::endl;
  std::cout << "     configuration_script.py  (required) python script to "
               "configure the processing"
            << std::endl;
  std::cout << "     arguments                (optional) passed to "
               "configuration script when run in python"
            << std::endl;
}
