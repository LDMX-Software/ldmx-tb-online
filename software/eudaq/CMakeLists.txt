# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.12)

# Set the project name
project(eudaq
	VERSION 0.1
	DESCRIPTION "Module containing all eudaq classes." 
	LANGUAGES CXX)

# EUDAQ loads modules at runtime based on what it "knows" about
#   a simple way to let EUDAQ know about our module is to put a symlink to our module
#   in the directory that the rest of the EUDAQ modules are in
# reference for creating a symlink: https://stackoverflow.com/a/42697475/17617632
#   - modified to restrict to our usecase
macro(symlink_to_eudaq name)
  set(origin "${CMAKE_INSTALL_PREFIX}/lib/libeudaq_${name}.so")
  set(dest   "${EUDAQ_LIBRARIES_DIRS}/libeudaq_module_ldmx_${name}.so")  
  install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${origin} ${dest})")
  install(CODE "message(\"-- Created symlink: ${dest} -> ${origin}\")")
endmacro()

setup_eudaq()
setup_rogue()

add_library(eudaq_core SHARED
  src/eudaq/CaptanTrigScintTestBeamProducer.cxx
  src/eudaq/DarkRunControl.cxx
  #src/eudaq/DipClientProducer.cxx
  src/eudaq/HCalDataSender.cxx
  src/eudaq/HCalTestBeamProducer.cxx
  src/eudaq/HgcrocDataPacket.cxx
  src/eudaq/HgcrocFileReaderProducer.cxx
  src/eudaq/monitor
  src/eudaq/RogueDataSender.cxx
  src/eudaq/RogueTcpClientProducer.cxx
  src/eudaq/TcpCommandGenerator.cxx
  src/eudaq/TestBeamDataCollector.cxx
  src/eudaq/TrigScintDataSender.cxx
  src/eudaq/TrigScintTestBeamProducer.cxx
  #src/eudaq/WRClientProducer.cxx
  )
target_include_directories(eudaq_core PUBLIC include)
target_link_libraries(eudaq_core PUBLIC EUDAQ::EUDAQ Rogue::Rogue rogue::rogue)
symlink_to_eudaq(core)

option(BUILD_MONITORING "Build online monitoring based off ROOT and EUDAQ" ON)
if (BUILD_MONITORING)
  find_package(ROOT REQUIRED COMPONENTS Gui Hist)
  add_library(eudaq_monitor SHARED
    src/eudaq/DarkMonitor.cxx
    src/eudaq/HCalTestBeamMonitor.cxx
    src/eudaq/SimpleMonitor.cxx
    src/eudaq/TestBeamEventDisplayMonitor.cxx
    src/eudaq/TestBeamMonitor.cxx
    src/eudaq/TrigScintTestBeamMonitor.cxx
    )
  target_link_libraries(eudaq_monitor PUBLIC EUDAQ::EUDAQ ROOT::Gui ROOT::Hist)
  symlink_to_eudaq(monitor)
  install(TARGETS eudaq_monitor DESTINATION lib)
endif()

install(TARGETS eudaq_core DESTINATION lib)
install(DIRECTORY include DESTINATION include)
