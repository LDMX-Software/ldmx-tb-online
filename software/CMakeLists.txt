# Set the minimum version of CMake that's required
cmake_minimum_required(VERSION 3.12)

# Set the name of the project
project(ldmx-tb-online
	VERSION 0.0.0
	DESCRIPTION "LDMX DAQ online software for testbeam." 
	LANGUAGES CXX)


# Add support for building in the conda environment 
if (DEFINED ENV{CONDA_PREFIX})
  set(CMAKE_PREFIX_PATH "$ENV{CONDA_PREFIX}")
  link_directories($ENV{CONDA_PREFIX}/lib)
endif()

# Rogue is required everywhere
if (DEFINED ENV{ROGUE_DIR})
  set(Rogue_DIR $ENV{ROGUE_DIR}/lib)
else()
  set(Rogue_DIR ${CMAKE_PREFIX_PATH}/lib)
endif()

# Define some colors. These are used to colorize CMake's user output
if(NOT WIN32)
  string(ASCII 27 esc)
  set(color_reset "${esc}[m")
  set(bold_yellow "${esc}[1;33m")
  set(green "${esc}[32m")
  set(bold_red "${esc}[1;31m")
endif()

# Override messages and add color
function(message)
  # empty message handling
  if ("${ARGV}" STREQUAL "")
    _message("")
    return()
  endif()

  # extract type of message from first argument
  list(GET ARGV 0 message_type)
  if(message_type STREQUAL FATAL_ERROR OR message_type STREQUAL SEND_ERROR)
    list(REMOVE_AT ARGV 0)
    _message("${bold_red}[ ERROR ]: ${ARGV}${color_reset}")
  elseif(message_type STREQUAL WARNING OR message_type STREQUAL AUTHOR_WARNING)
    list(REMOVE_AT ARGV 0)
    _message("${bold_yellow}[ WARNING ]: ${ARGV}${color_reset}")
  elseif(message_type STREQUAL STATUS)
    list(REMOVE_AT ARGV 0)
    _message("${green}[ INFO ]: ${ARGV}${color_reset}")
  else()
    _message("${green} ${ARGV} ${color_reset}")
  endif()
endfunction()

# find Rogue package and put it into a cmake target
find_package(Rogue CONFIG REQUIRED)
add_library(Rogue::Rogue INTERFACE IMPORTED GLOBAL)
set_target_properties(Rogue::Rogue PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${ROGUE_INCLUDE_DIRS}"
  INTERFACE_LINK_LIBRARIES "${ROGUE_LIBRARIES}")

# If an install location hasn't been set via CMAKE_INSTALL_PREFIX, set it to 
# a reasonable default ($PWD/install).  
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/install CACHE PATH "" FORCE)
  message(STATUS "Install directory set to ${CMAKE_INSTALL_PREFIX}")
endif()

# Set the default release type to "Release". If a release type is specified 
# at the command line, it's respected.
set(default_build_type "Release")
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
endif()

add_subdirectory(emulators)

option(EMULATORS_ONLY "Only build the emulators" OFF)
if (NOT EMULATORS_ONLY) 
  add_subdirectory(rogue)
  add_subdirectory(eudaq)
  add_subdirectory(utils)
endif() 
