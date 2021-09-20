# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.12)

# Set the project name
project(emulators
	VERSION 0.1
	DESCRIPTION "Module containing all emulator classes." 
	LANGUAGES CXX)

# Setup the Rogue CMake target
setup_rogue()

# Build the emulator library
#setup_library(module emulators dependencies Rogue::Rogue)

add_executable(rce_prbs_tcp_server ${PROJECT_SOURCE_DIR}/src/emulators/rce_prbs_tcp_server.cxx)

target_link_libraries(rce_prbs_tcp_server
	              PRIVATE Rogue::Rogue)

install(TARGETS rce_prbs_tcp_server DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

add_executable(rce_prbs_tcp_client ${PROJECT_SOURCE_DIR}/src/emulators/rce_prbs_tcp_client.cxx)

target_link_libraries(rce_prbs_tcp_client
	              PRIVATE Rogue::Rogue)

install(TARGETS rce_prbs_tcp_client DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
