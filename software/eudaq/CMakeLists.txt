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

# Build the eudaq library
#setup_library(module eudaq name eudaq_module_dark 
#	      dependencies EUDAQ::EUDAQ Rogue::Rogue rogue::rogue
#	      ROOT::Gui ROOT::Hist)


setup_library(module eudaq name eudaq_module_dark 
  dependencies EUDAQ::EUDAQ Rogue::Rogue rogue::rogue
  ROOT::Gui ROOT::Hist FiberTrackerDAQ)


#Add fiber tracker -- TODO
target_include_directories(eudaq_module_dark PUBLIC ${FIBERTRACKERDAQ_Dir}/include ${DIP_Dir}/include)
target_link_directories(eudaq_module_dark PUBLIC ${FIBERTRACKERDAQ_Dir}/lib ${DIP_Dir}/lib64/)

