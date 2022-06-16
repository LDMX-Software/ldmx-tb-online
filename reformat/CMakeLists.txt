
# Set the minimum version of CMake that's required 
cmake_minimum_required(VERSION 3.12)

# Set the project name
project(reformat VERSION 0.0.0
  DESCRIPTION "Reformatting raw data into the Framework EventFile"
  LANGUAGES CXX    
)

# Load additional macros used by this project. 
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/)

# Load the BuildMacros module. If loaded correctly, the variable 
# 'build_macros_found' will be set to the path of the module.  Otherwise, it
# is set to NOTFOUND.
include(BuildMacros RESULT_VARIABLE build_macros_found)

# If an install location hasn't been set via CMAKE_INSTALL_PREFIX, set it to 
# a reasonable default ($LDMX_BASE/ldmx-sw/install).  
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX $ENV{LDMX_BASE}/ldmx-sw/install CACHE PATH "" FORCE)
  message(STATUS "Install directory set to ${CMAKE_INSTALL_PREFIX}")
endif()

# Set the default release type to "Release". If a release type is specified 
# at the command line, it's respected.
set(default_build_type "Release")
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
endif()

# Clear any variables cached during previous configuration cycles. 
clear_cache_variables()

# The framework has all the backend code to write EventFiles
#   if it is already installed, simply wrap interfaces with it
#   otherwise build it ourselves
if (EXISTS $ENV{LDMX_BASE}/ldmx-sw/install/include/Framework/Version.h)
  find_package(ROOT 6.16 CONFIG REQUIRED)
  find_package(Python3 COMPONENTS Interpreter Development)
  find_package(Boost REQUIRED COMPONENTS log)
  add_library(Framework INTERFACE)
  target_link_libraries(Framework INTERFACE 
    $ENV{LDMX_BASE}/ldmx-sw/install/lib/libFramework.so
    Python3::Python 
    Boost::log
    ROOT::Core)
  target_include_directories(Framework INTERFACE $ENV{LDMX_BASE}/ldmx-sw/install/include)
  add_library(Framework::Framework ALIAS Framework)
else()
  add_subdirectory(Framework)
endif()

# testing package
include(CTest)

# helper classes for interfacing between EventFiles and Raw data channels
add_subdirectory(Reformat)

# TestBeam intermediate bandaids
add_subdirectory(TestBeam)
