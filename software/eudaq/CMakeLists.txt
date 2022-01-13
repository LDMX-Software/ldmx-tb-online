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
