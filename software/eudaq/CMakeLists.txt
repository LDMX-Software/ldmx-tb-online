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

# Build the emulator library
setup_library(module eudaq name eudaq_module_dark dependencies EUDAQ::EUDAQ Rogue::Rogue)
