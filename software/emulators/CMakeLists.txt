# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.12)

# Set the project name
project(emulators
	DESCRIPTION "Module containing all emulator classes." 
	LANGUAGES CXX)

# Search for Boost
find_package(Boost REQUIRED COMPONENTS program_options)

# Build the emulator library
add_library(emulators SHARED 
  src/emulators/HCalGenerator.cxx
  src/emulators/HCalReceiver.cxx
  src/emulators/QieDataPacket.cxx
  src/emulators/TrigScintGenerator.cxx
  src/emulators/TrigScintReceiver.cxx
  src/emulators/TrigScint_SlowControlGenerator.cxx
  src/emulators/TrigScint_SlowControlReceiver.cxx
  )
target_link_libraries(emulators PUBLIC Rogue::Rogue)
target_include_directories(emulators PUBLIC include)

add_executable(emulator app/emulator.cxx)
target_link_libraries(emulator PRIVATE Boost::program_options Rogue::Rogue emulators)

add_executable(rce_prbs_tcp_client app/rce_prbs_tcp_client.cxx)
target_link_libraries(rce_prbs_tcp_client PRIVATE Rogue::Rogue)

add_executable(rce_prbs_tcp_server app/rce_prbs_tcp_server.cxx)
target_link_libraries(rce_prbs_tcp_server PRIVATE Rogue::Rogue)

install(TARGETS emulators rce_prbs_tcp_client rce_prbs_tcp_server emulator 
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)
