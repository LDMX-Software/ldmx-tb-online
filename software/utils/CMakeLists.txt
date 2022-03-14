# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.12)

add_executable(sendTsControlCommand src/sendTsControlCommand.cxx)
target_link_libraries(sendTsControlCommand PRIVATE eudaq_core rogue)

add_executable(sendTsConfigCommand src/sendTsConfigCommand.cxx)
target_link_libraries(sendTsConfigCommand PRIVATE eudaq_core rogue)

add_executable(receiveUdpTsPackets src/receiveUdpTsPackets.cxx)
target_link_libraries(receiveUdpTsPackets PRIVATE rogue)

install(TARGETS sendTsConfigCommand sendTsControlCommand receiveUdpTsPackets 
  DESTINATION ${CMAKE_INSTALL_PREFIX}/utils/)

