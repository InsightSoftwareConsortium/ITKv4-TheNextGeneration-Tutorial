cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(ITKv4-TheNextGeneration)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY
  ${ITKv4-TheNextGeneration_BINARY_DIR}/bin
)

file(GLOB_RECURSE CXX_FILES "Exercises/*.cxx")

option(BUILD_PRESENTATION "Build the PDF Presentation." OFF)

if(BUILD_PRESENTATION)
  add_subdirectory(Documents)
endif()

option(BUILD_EXERCISES "Build the Exercises." OFF)

if(BUILD_EXERCISES)
  add_subdirectory(Exercises)
endif()
