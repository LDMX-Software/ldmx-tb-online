# Set the project name
project(TestBeam)

add_library(TestBeam SHARED
  src/TestBeam/PolarfireRawFile.cxx
  src/TestBeam/WhiteRabbitRawFile.cxx
  src/TestBeam/FiberTrackerRawFile.cxx
  )
target_link_libraries(TestBeam PUBLIC Reformat::Reformat Reformat::Utility)
install(TARGETS TestBeam DESTINATION lib)

setup_python(package_name Reformat)
