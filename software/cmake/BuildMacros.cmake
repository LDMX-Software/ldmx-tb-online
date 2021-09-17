include(CMakeParseArguments)

macro(setup_rogue)
  
  # If the target doesn't exist, create an imported target for Rogue
  if(NOT TARGET Rogue::Rogue)
    
    # Find the Rogue & support libraries
    if (DEFINED ENV{ROGUE_DIR})
       set(Rogue_DIR $ENV{ROGUE_DIR}/lib)
    else()
       set(Rogue_DIR ${CMAKE_PREFIX_PATH}/lib)
    endif()
    find_package(Rogue CONFIG REQUIRED)

    # If Rogue wasn't found, error out.
    if (NOT Rogue_FOUND)
      message(FATAL_ERROR "Failed to find required dependency Rogue.")
    endif()

    # Create the Rogue target
    add_library(Rogue::Rogue INTERFACE IMPORTED GLOBAL)

    # Need to remove the keyword PUBLIC from the list of libraries to avoid 
    # an ld error. 
    list(REMOVE_ITEM ROGUE_LIBRARIES "PUBLIC")
    # Set the target properties
    set_target_properties(Rogue::Rogue
	    PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${ROGUE_INCLUDE_DIRS}"
	    INTERFACE_LINK_LIBRARIES "${ROGUE_LIBRARIES}")
  endif()
endmacro()

macro(setup_library)

  set(oneValueArgs module)
  set(multiValueArgs dependencies sources)
  cmake_parse_arguments(setup_library "${options}" "${oneValueArgs}"
                        "${multiValueArgs}" ${ARGN})

  # Build the library name and source path
  set(library_name "${setup_library_module}")
  set(src_path "${PROJECT_SOURCE_DIR}/src/${setup_library_module}")
  set(include_path "include/${setup_library_module}")

  # Find all of the source files we want to add to the library
  if(NOT setup_library_sources)
    file(GLOB SRC_FILES CONFIGURE_DEPENDS ${src_path}/[a-zA-Z]*.cxx)
  else()
    set(SRC_FILES ${setup_library_sources})
  endif()

  # Add the sources to the library
  add_library(${library_name} SHARED ${SRC_FILES})
  target_include_directories(${library_name}
                             PUBLIC ${PROJECT_SOURCE_DIR}/include)

  # Setup the targets to link against
  target_link_libraries(${library_name} PUBLIC ${setup_library_dependencies})

  # Define an alias. This is used to create the imported target.
  set(alias "${setup_library_module}::${setup_library_module}")
  add_library(${alias} ALIAS ${library_name})

  # Install the libraries and headers
  install(TARGETS ${library_name}
          LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
  install(DIRECTORY ${PROJECT_SOURCE_DIR}/${include_path}
          DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

endmacro()

macro(setup_python)

  set(oneValueArgs package_name)
  cmake_parse_arguments(setup_python "${options}" "${oneValueArgs}"
                        "${multiValueArgs}" ${ARGN})

  # If the python directory exists, initialize the package and copy over the
  # python modules.
  if(EXISTS ${PROJECT_SOURCE_DIR}/python)

    # Get a list of all python files inside the python package
    file(GLOB py_scripts CONFIGURE_DEPENDS
         ${PROJECT_SOURCE_DIR}/python/[a-zA-Z]*.py
         ${PROJECT_SOURCE_DIR}/python/[a-zA-Z]*.py.in)

    foreach(pyscript ${py_scripts})

      # If a filename has a '.in' extension, remove it.  The '.in' extension is
      # used to denote files that have variables that will be substituded by the
      # configure_file macro.
      string(REPLACE ".in" "" script_output ${pyscript})

      # GLOB returns the full path to the file.  We also need the filename so
      # it's new location can be specified.
      get_filename_component(script_output ${script_output} NAME)

      # Copy the file from its original location to the bin directory.  This
      # will also replace all cmake variables within the files.
      configure_file(${pyscript}
                     ${CMAKE_CURRENT_BINARY_DIR}/python/${script_output})

      # Install the files to the given path
      install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/python/${script_output}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/python/${setup_python_package_name})
    endforeach()

  endif()

endmacro()

macro(setup_test)

  set(multiValueArgs dependencies)
  cmake_parse_arguments(setup_test "${options}" "${oneValueArgs}"
                        "${multiValueArgs}" ${ARGN})

  # Find all the test
  file(GLOB src_files CONFIGURE_DEPENDS
       ${PROJECT_SOURCE_DIR}/test/[a-zA-Z]*.cxx)
  file(GLOB py_files CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/test/[a-zA-Z]*.py)

  # If the directory contains python unit test of configurations, copy them over
  # to the test directory within the build directory.
  if(py_files)
    file(COPY ${py_files} DESTINATION ${CMAKE_BINARY_DIR}/test)
  endif()

  # Add all test to the global list of test sources
  set(test_sources
      ${test_sources} ${src_files}
      CACHE INTERNAL "test_sources")

  # Add all of the dependencies to the global list of dependencies
  set(test_dep
      ${test_dep} ${setup_test_dependencies}
      CACHE INTERNAL "test_dep")

  # Add the module to the list of tags
  get_filename_component(module ${PROJECT_SOURCE_DIR} NAME)
  set(test_modules
      ${test_modules} ${module}
      CACHE INTERNAL "test_modules")

endmacro()

macro(build_test)

  enable_testing()

  # If test have been enabled, attempt to find catch.  If catch hasn't found, it
  # will be downloaded locally.
  find_package(Catch2 2.13.0)

  # Create the Catch2 main exectuable if it doesn't exist
  if(NOT EXISTS ${CMAKE_BINARY_DIR}/test/run_test.cxx)

    file(WRITE ${CMAKE_BINARY_DIR}/test/run_test.cxx
         "#define CATCH_CONFIG_MAIN\n#include \"catch.hpp\"")

    message(
      STATUS "Writing Catch2 main to: ${CMAKE_BINARY_DIR}/test/run_test.cxx")
  endif()

  # Add the executable to run all test.  test_sources is a cached variable that
  # contains the test from the different modules.  Each of the modules needs to
  # setup the test they want to run.
  add_executable(run_test ${CMAKE_BINARY_DIR}/test/run_test.cxx ${test_sources})
  target_include_directories(run_test PRIVATE ${CATCH2_INCLUDE_DIR})
  target_link_libraries(run_test PRIVATE Catch2::Interface ${test_dep})

  # Install the run_test  executable
  foreach(entry ${test_modules})
    add_test(
      NAME ${entry}
      COMMAND run_test "[${entry}]"
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test)
  endforeach()

endmacro()

macro(clear_cache_variables)
  unset(test_sources CACHE)
  unset(test_dep CACHE)
  unset(test_modules CACHE)
  unset(namespaces CACHE)
endmacro()
