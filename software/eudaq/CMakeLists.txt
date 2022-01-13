# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.12)

# Set the project name
project(eudaq
	VERSION 0.1
	DESCRIPTION "Module containing all eudaq classes." 
	LANGUAGES CXX)

# Setup the Rogue CMake target
setup_eudaq()

# Setup the Rogue CMake target
setup_rogue()

# Find all of the source files we want to add to the eudaq library.
# For now, exclude the DIP client until it can be brought in as 
# an external dependency.
file(GLOB SRC_FILES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/eudaq/[A-Z]*.cxx)
list(REMOVE_ITEM SRC_FILES ${PROJECT_SOURCE_DIR}/src/eudaq/DipClientProducer.cxx)

setup_library(module eudaq name eudaq_module_dark 
  dependencies EUDAQ::EUDAQ Rogue::Rogue rogue::rogue
  sources ${SRC_FILES})
#FiberTrackerDAQ)

# EUDAQ loads modules at runtime based on what it "knows" about
#   a simple way to let EUDAQ know about our module is to put a symlink to our module
#   in the directory that the rest of the EUDAQ modules are in
# reference for creating a symlink: https://stackoverflow.com/a/41037224/17617632
install(CODE "execute_process( \
    COMMAND ${CMAKE_COMMAND} -E create_symlink \
    ${CMAKE_INSTALL_PREFIX}/lib/libeudaq_module_dark.so \
    ${EUDAQ_LIBRARIES_DIRS}/libeudaq_module_dark.so \
    )")
install(CODE "message(\"-- Created symlink: \
    ${CMAKE_INSTALL_PREFIX}/lib/libeudaq_module_dark.so -> \
    ${EUDAQ_LIBRARIES_DIRS}/libeudaq_module_dark.so \
    )")

#Add fiber tracker -- TODO
#target_include_directories(eudaq_module_dark PUBLIC ${FIBERTRACKERDAQ_Dir}/include ${DIP_Dir}/include)
#target_link_directories(eudaq_module_dark PUBLIC ${FIBERTRACKERDAQ_Dir}/lib ${DIP_Dir}/lib64/)

find_package(ROOT CONFIG)
if (ROOT_FOUND)
  # we assume we can enable the monitoring
  setup_library(module eudaq name monitor
    dependencies EUDAQ::EUDAQ Rogue::Rogue rogue::rogue
      ROOT::Gui ROOT::Hist)
else()
  # no ROOT -> no monitoring
  message(WARNING "ROOT not found, so monitoring is disabled.")
endif()
