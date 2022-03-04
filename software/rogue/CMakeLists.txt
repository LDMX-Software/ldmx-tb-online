# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.12)

# Set the project name
project(rogue
  DESCRIPTION "LDMX TB Rogue stuff"
	LANGUAGES CXX)

# Search for Boost
find_package(Boost REQUIRED COMPONENTS program_options)

# Find all of the source files we want to add to the emulators library.
# Note, this excludes all executables which have '_' in the name.
file(GLOB SRC_FILES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/rogue/[A-Z]*.cxx)
add_library(rogue SHARED ${SRC_FILES})
target_include_directories(rogue PUBLIC include)
target_link_libraries(rogue PUBLIC Rogue::Rogue emulators)

add_executable(ldmx_rogue_server src/rogue/ldmx_rogue_server.cxx)
target_link_libraries(ldmx_rogue_server PRIVATE Boost::program_options Rogue::Rogue rogue)

install(TARGETS rogue ldmx_rogue_server 
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)
