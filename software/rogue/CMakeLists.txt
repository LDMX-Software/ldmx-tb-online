# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.12)

# Set the project name
project(rogue
  DESCRIPTION "LDMX TB Rogue stuff"
	LANGUAGES CXX)

# Search for Boost
find_package(Boost REQUIRED COMPONENTS program_options)

add_library(rogue SHARED
  src/rogue/TcpCommandReceiver.cxx
  src/rogue/TrigScintReceiver.cxx
  src/rogue/TsUdpClient.cxx
  src/rogue/TBStreamWriter.cxx
  )
target_include_directories(rogue PUBLIC include)
target_link_libraries(rogue PUBLIC Rogue::Rogue emulators)

add_executable(ldmx_rogue_server app/ldmx_rogue_server.cxx)
target_link_libraries(ldmx_rogue_server PRIVATE Boost::program_options Rogue::Rogue rogue)

install(TARGETS rogue ldmx_rogue_server 
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)
