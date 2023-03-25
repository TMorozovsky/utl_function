cmake_minimum_required(VERSION 3.16)

project(utl_function_tests)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# Find GTest

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(GTest REQUIRED)


# Add a unit tests project

add_executable(utl_function_tests
               override_global_new_and_delete.hpp
               tested_functions_and_function_objects.hpp
               basic_utl_function_test_fixture.hpp
               static_assertions.cpp
               test_utl_function_case_01_default_construction.cpp
               test_utl_function_case_02_construction_from_callables_and_invocation.cpp
               test_utl_function_case_03_move_construction.cpp
               test_utl_function_case_04_assignment_from_callables.cpp)

target_include_directories(utl_function_tests PRIVATE "..")

set_property(TARGET utl_function_tests PROPERTY CXX_STANDARD 20)

target_link_libraries(utl_function_tests PRIVATE GTest::gtest_main)

add_custom_command(TARGET utl_function_tests
                   POST_BUILD
                   COMMAND "$<TARGET_FILE:utl_function_tests>")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT utl_function_tests)


# Visual Studio-specific: add the library itself as a target in order to be more IDE-friendly
if (CMAKE_GENERATOR MATCHES "Visual Studio")
  get_filename_component(UTL_FILES_ABSOLUTE_PATH "../utl" ABSOLUTE)

  file(GLOB_RECURSE UTL_ALL_FILES
       LIST_DIRECTORIES FALSE
       "${UTL_FILES_ABSOLUTE_PATH}/*")

  foreach(UTL_FILE IN ITEMS ${UTL_ALL_FILES})
      get_filename_component(UTL_FILE_PATH "${UTL_FILE}" PATH)
      file(RELATIVE_PATH UTL_FILE_RELATIVE_PATH "${UTL_FILES_ABSOLUTE_PATH}" "${UTL_FILE_PATH}")
      string(REPLACE "/" "\\" UTL_FILE_GROUP_PATH "${UTL_FILE_RELATIVE_PATH}")
      source_group("${UTL_FILE_GROUP_PATH}" FILES "${UTL_FILE}")
  endforeach()

  add_library(utl INTERFACE ${UTL_ALL_FILES})

  set_property(TARGET utl PROPERTY CXX_STANDARD 20)
endif()
