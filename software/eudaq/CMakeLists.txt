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

# Setup the Rogue CMake target
setup_eudaq()

# Setup the Rogue CMake target
setup_rogue()

add_library(eudaq_core SHARED 
  src/eudaq/DarkRunControl.cxx
  src/eudaq/HgcrocFileReaderProducer.cxx
  src/eudaq/HgcrocDataPacket.cxx
  src/eudaq/HCalDataSender.cxx
  src/eudaq/HCalTestBeamProducer.cxx
  src/eudaq/PolarfireProducer.cxx
  src/eudaq/RogueDataSender.cxx
  src/eudaq/RogueTcpClientProducer.cxx
  src/eudaq/TcpCommandGenerator.cxx
  src/eudaq/TestBeamDataCollector.cxx
  src/eudaq/TrigScintDataSender.cxx
  src/eudaq/TrigScintTestBeamProducer.cxx)
target_include_directories(eudaq_core PUBLIC include)
target_link_libraries(eudaq_core PUBLIC EUDAQ::EUDAQ Rogue::Rogue rogue::rogue pflib::rogue)
add_library(eudaq::eudaq ALIAS eudaq_core)
symlink_to_eudaq(core)

# For now, exclude the DIP client until it can be brought in as an external dependency
#add_library(eudaq_dip SHARED
#  src/eudaq/dip/DipClientProducer.cxx)
#target_include_directories(eudaq_dip PUBLIC include)
#target_link_libraries(eudaq_dip PUBLIC eudaq_core)
#add_library(eudaq::dip ALIAS eudaq_dip)
#symlink_to_eudaq(dip)

#Add fiber tracker -- TODO
#target_include_directories(eudaq_dip PUBLIC ${FIBERTRACKERDAQ_Dir}/include ${DIP_Dir}/include)
#target_link_directories(eudaq_dip PUBLIC ${FIBERTRACKERDAQ_Dir}/lib ${DIP_Dir}/lib64/)

option(MONITORING "Build monitoring submodule, requires ROOT" OFF)
if (MONITORING)
  find_package(ROOT CONFIG REQUIRED)
  # we assume we can enable the monitoring
  set(monitor_target eudaq_monitor)
  add_library(eudaq_monitor SHARED
    src/eudaq/monitor/HCalTestBeamMonitor.cxx
    src/eudaq/monitor/SimpleMonitor.cxx
    src/eudaq/monitor/TrigScintTestBeamMonitor.cxx)
  target_include_directories(eudaq_hcal PUBLIC include)
  target_link_libraries(eudaq_hcal PUBLIC eudaq_core ROOT::Gui ROOT::Hist)
  add_library(eudaq::hcal ALIAS eudaq_hcal)
  symlink_to_eudaq(hcal)
else()
  message(STATUS "Not building eudaq::monitoring")
endif()

install(TARGETS eudaq_core ${monitor_target} LIBRARY DESTINATION lib)
install(DIRECTORY include DESTINATION include)

