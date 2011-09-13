cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(ITKv4-TheNextGeneration)

file(GLOB_RECURSE CXX_FILES "*.cxx")

add_subdirectory(Exercises)

option(BUILD_PRESENTATION "Build the PDF Presentation." ON)

if(BUILD_PRESENTATION)
  add_subdirectory(Documents)
endif()
