# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.12)

# Setup the Rogue CMake target
setup_rogue()

add_executable(sendTsControlCommand ${CMAKE_BINARY_DIR}/../utils/src/sendTsControlCommand.cxx)
target_link_libraries(sendTsControlCommand
  PRIVATE eudaq_module_dark rogue::rogue)

add_executable(sendTsConfigCommand ${CMAKE_BINARY_DIR}/../utils/src/sendTsConfigCommand.cxx)

target_link_libraries(sendTsConfigCommand
  PRIVATE eudaq_module_dark rogue::rogue)

install(TARGETS sendTsControlCommand sendTsConfigCommand DESTINATION ${CMAKE_INSTALL_PREFIX}/utils/)
