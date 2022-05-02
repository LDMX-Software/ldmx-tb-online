# Set the project name
project(TestBeam)

# Make a library
setup_library(module TestBeam
              dependencies Reformat::Reformat
                           Reformat::Utility)

setup_python(package_name Reformat)
