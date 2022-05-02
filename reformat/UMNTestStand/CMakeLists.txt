# Set the project name
project(UMNTestStand)

# Make a library
setup_library(module UMNTestStand
              dependencies Reformat::Reformat
                           Reformat::Utility)

setup_python(package_name Reformat)
