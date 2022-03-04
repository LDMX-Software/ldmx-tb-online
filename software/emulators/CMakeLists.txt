# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.12)

# Set the project name
project(emulators
	VERSION 0.1
	DESCRIPTION "Module containing all emulator classes." 
	LANGUAGES CXX)

# Search for Boost
find_package(Boost REQUIRED COMPONENTS program_options)

# Find all of the source files we want to add to the emulators library.
# Note, this excludes all executables which have '_' in the name.
file(GLOB SRC_FILES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/emulators/[A-Z]*.cxx)

# Build the emulator library
add_library(emulators SHARED ${SRC_FILES})
target_link_libraries(emulators PUBLIC Rogue::Rogue)
target_include_directories(emulators PUBLIC include)

add_executable(emulator src/emulators/emulator.cxx)
target_link_libraries(emulator PRIVATE Boost::program_options Rogue::Rogue emulators)

add_executable(rce_prbs_tcp_client src/emulators/rce_prbs_tcp_client.cxx)
target_link_libraries(rce_prbs_tcp_client PRIVATE Rogue::Rogue)

install(TARGETS emulators rce_prbs_tcp_client emulator 
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)
