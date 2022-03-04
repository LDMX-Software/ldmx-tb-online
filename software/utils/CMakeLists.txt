# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.12)

# Setup the Rogue CMake target
setup_rogue()

add_executable(sendTsControlCommand ${CMAKE_BINARY_DIR}/../utils/src/sendTsControlCommand.cxx)
target_link_libraries(sendTsControlCommand PRIVATE eudaq_core rogue::rogue)

add_executable(sendTsConfigCommand ${CMAKE_BINARY_DIR}/../utils/src/sendTsConfigCommand.cxx)
target_link_libraries(sendTsConfigCommand PRIVATE eudaq_core rogue::rogue)

add_executable(receiveUdpTsPackets ${CMAKE_BINARY_DIR}/../utils/src/receiveUdpTsPackets.cxx)
target_link_libraries(receiveUdpTsPackets PRIVATE rogue::rogue)

install(TARGETS sendTsConfigCommand sendTsControlCommand receiveUdpTsPackets DESTINATION ${CMAKE_INSTALL_PREFIX}/utils/)


