# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.12)

# Set the project name
project(rogue
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
file(GLOB SRC_FILES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/rogue/[A-Z]*.cxx)

# Build the emulator library
setup_library(module rogue dependencies Rogue::Rogue emulators::emulators
	sources ${SRC_FILES})

add_executable(ldmx_rogue_server ${PROJECT_SOURCE_DIR}/src/rogue/ldmx_rogue_server.cxx)

target_link_libraries(ldmx_rogue_server
	              PRIVATE Boost::program_options Rogue::Rogue rogue::rogue)

install(TARGETS ldmx_rogue_server DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
