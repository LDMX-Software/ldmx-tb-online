# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.12)

find_package(ROOT CONFIG REQUIRED)


add_executable(testTcpCommandGenerator ${CMAKE_BINARY_DIR}/../test/src/testTcpCommandGenerator.cxx)
target_link_libraries(testTcpCommandGenerator
  PRIVATE eudaq_module_dark ROOT::Gui -lm -ldl)


add_executable(testTsServer ${CMAKE_BINARY_DIR}/../test/src/testTsServer.cxx)
target_link_libraries(testTsServer
  PRIVATE eudaq_module_dark ROOT::Gui -lm -ldl)



install(TARGETS testTcpCommandGenerator DESTINATION ${CMAKE_INSTALL_PREFIX}/test/)
