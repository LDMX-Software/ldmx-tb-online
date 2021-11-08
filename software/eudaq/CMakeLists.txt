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

find_package(ROOT CONFIG REQUIRED)

# Find all of the source files we want to add to the eudaq library.
# For now, exclude the DIP client until it can be brought in as 
# an external dependency.
file(GLOB SRC_FILES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/eudaq/[A-Z]*.cxx)
list(REMOVE_ITEM SRC_FILES ${PROJECT_SOURCE_DIR}/src/eudaq/DipClientProducer.cxx)

setup_library(module eudaq name eudaq_module_dark 
  dependencies EUDAQ::EUDAQ Rogue::Rogue rogue::rogue
  ROOT::Gui ROOT::Hist
  sources ${SRC_FILES})
#FiberTrackerDAQ)


#Add fiber tracker -- TODO
#target_include_directories(eudaq_module_dark PUBLIC ${FIBERTRACKERDAQ_Dir}/include ${DIP_Dir}/include)
#target_link_directories(eudaq_module_dark PUBLIC ${FIBERTRACKERDAQ_Dir}/lib ${DIP_Dir}/lib64/)

