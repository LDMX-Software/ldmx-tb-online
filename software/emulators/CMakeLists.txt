# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.12)

# Set the project name
project(emulators
	VERSION 0.1
	DESCRIPTION "Module containing all emulator classes." 
	LANGUAGES CXX)

# Search for Boost
find_package(
  Boost REQUIRED
  COMPONENTS program_options)

# Setup the Rogue CMake target
setup_rogue()

# Find all of the source files we want to add to the emulators library.
# Note, this excludes all executables which have '_' in the name.
file(GLOB SRC_FILES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/emulators/[A-Z]*.cxx)

# Build the emulator library
setup_library(module emulators dependencies Rogue::Rogue
	sources ${SRC_FILES})

add_executable(emulator ${PROJECT_SOURCE_DIR}/src/emulators/emulator.cxx)

target_link_libraries(emulator
	              PRIVATE Boost::program_options Rogue::Rogue emulators::emulators)

install(TARGETS emulator DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

add_executable(rce_prbs_tcp_client ${PROJECT_SOURCE_DIR}/src/emulators/rce_prbs_tcp_client.cxx)

target_link_libraries(rce_prbs_tcp_client
	              PRIVATE Rogue::Rogue)

install(TARGETS rce_prbs_tcp_client DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
