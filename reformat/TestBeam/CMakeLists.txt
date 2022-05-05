# Set the project name
project(TestBeam)

add_library(TestBeam SHARED
  src/TestBeam/PolarfireRawFile.cxx)
target_link_libraries(TestBeam PUBLIC Reformat::Reformat Reformat::Utility)

setup_python(package_name Reformat)
