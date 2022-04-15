# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.12)

# Set the project name
project(eudaq
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

# Find the eudaq libraries
if (DEFINED ENV{EUDAQ_DIR})
  set(eudaq_DIR $ENV{EUDAQ_DIR}/cmake)
endif()
find_package(eudaq CONFIG REQUIRED)
add_library(EUDAQ::EUDAQ INTERFACE IMPORTED GLOBAL)
foreach(eudaq_lib ${EUDAQ_LIBRARIES})
  list(APPEND eudaq_libs "${EUDAQ_LIBRARIES_DIRS}/lib${eudaq_lib}.so")
endforeach()
set_target_properties(EUDAQ::EUDAQ PROPERTIES 
  INTERFACE_INCLUDE_DIRECTORIES "${EUDAQ_INCLUDE_DIRS}"
  INTERFACE_LINK_LIBRARIES "${eudaq_libs}")

add_library(eudaq_core SHARED
  src/eudaq/CaptanTrigScintTestBeamProducer.cxx
  src/eudaq/DarkRunControl.cxx
  src/eudaq/HgcrocRawDecode.cxx
  src/eudaq/HgcrocFileReaderProducer.cxx
  src/eudaq/TrigScintFileReaderProducer.cxx
  src/eudaq/RogueDataSender.cxx
  src/eudaq/RogueTcpClientProducer.cxx
  src/eudaq/TcpCommandGenerator.cxx
  src/eudaq/TestBeamDataCollector.cxx
  src/eudaq/TrigScintRawDecode.cxx
  src/eudaq/SimQIE.cxx
  src/eudaq/TrigScintDataSender.cxx
  src/eudaq/TrigScintTestBeamProducer.cxx
  src/eudaq/SimQIE.cxx
  )
target_include_directories(eudaq_core PUBLIC include)
target_link_libraries(eudaq_core PUBLIC EUDAQ::EUDAQ Rogue::Rogue rogue)
symlink_to_eudaq(core)
install(TARGETS eudaq_core DESTINATION lib)

if (NOT WRTiming_DIR)
  set(WRTiming_DIR $ENV{WRTiming_DIR})
endif()

if (EXISTS ${WRTiming_DIR}/include/WRClient.h)
  add_library(eudaq_wr SHARED src/eudaq/wr/WRClientProducer.cxx)
  target_include_directories(eudaq_wr PUBLIC ${WRTiming_DIR}/include)
  target_link_directories(eudaq_wr PUBLIC ${WRTiming_DIR}/build/)
  target_link_libraries(eudaq_wr PUBLIC Timestamp EUDAQ::EUDAQ)	
  symlink_to_eudaq(wr)
  install(TARGETS eudaq_wr DESTINATION lib)
else()
  message(WARNING "Unable to find WRClient - WRClientProducer will not be built.")
endif()

if (NOT FiberTrackerDAQ_DIR)
  set(FiberTrackerDAQ_DIR $ENV{FiberTrackerDAQ_DIR})
endif()

if (NOT dip_DIR)
  set(dip_DIR $ENV{dip_DIR})
endif()

if (EXISTS ${FiberTrackerDAQ_DIR}/include/FiberTrackerClient.h AND EXISTS ${dip_DIR}/include/Dip.h)
  add_library(eudaq_dip SHARED src/eudaq/dip/DipClientProducer.cxx)
  target_include_directories(eudaq_dip PUBLIC ${FiberTrackerDAQ_DIR}/include ${dip_DIR}/include)
  target_link_libraries(eudaq_dip PUBLIC EUDAQ::EUDAQ
    ${FiberTrackerDAQ_DIR}/lib/libFiberTrackerDAQ.so
    ${dip_DIR}/lib64/libdip.so)
  symlink_to_eudaq(dip)
  install(TARGETS eudaq_dip DESTINATION lib)
else()
  message(WARNING "Unable to find FiberTrackerClient.h or Dip.h - DipClientProducer will not be built.")
endif()

find_package(pflib 2.0.4 QUIET)
if (pflib_FOUND)
  add_library(eudaq_pflib SHARED
    src/eudaq/pflib/PolarfireProducer.cxx)
  target_link_libraries(eudaq_pflib PUBLIC eudaq_core pflib::rogue)
  symlink_to_eudaq(pflib)
  install(TARGETS eudaq_pflib DESTINATION lib)
else()
  message(WARNING "pflib version >= 2 not found - PolarfireProducer will not be available.")
endif()

option(BUILD_MONITORING "Build online monitoring based off ROOT and EUDAQ" ON)
if (BUILD_MONITORING)
  find_package(ROOT REQUIRED COMPONENTS Gui Hist)
  add_library(eudaq_monitor SHARED
    src/eudaq/monitor/HCalTestBeamMonitor.cxx
    src/eudaq/monitor/SimpleMonitor.cxx
    src/eudaq/monitor/TestBeamEventDisplayMonitor.cxx
    src/eudaq/monitor/TrigScintTestBeamMonitor.cxx
    )
  target_include_directories(eudaq_monitor PUBLIC include)
  target_link_libraries(eudaq_monitor PUBLIC eudaq_core ROOT::Gui ROOT::Hist)
  symlink_to_eudaq(monitor)
  install(TARGETS eudaq_monitor DESTINATION lib)
endif()

