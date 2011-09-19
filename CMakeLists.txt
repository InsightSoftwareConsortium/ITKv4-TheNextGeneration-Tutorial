cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(ITKv4-TheNextGeneration)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY
  ${ITKv4-TheNextGeneration_BINARY_DIR}/bin
)

file(GLOB_RECURSE CXX_FILES "*.cxx")

add_subdirectory(Exercises)

option(BUILD_PRESENTATION "Build the PDF Presentation." OFF)

if(BUILD_PRESENTATION)
  add_subdirectory(Documents)
endif()
