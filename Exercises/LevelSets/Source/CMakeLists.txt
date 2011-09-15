cmake_minimum_required(VERSION 2.4)
if(COMMAND CMAKE_POLICY)
  cmake_policy(SET CMP0003 NEW)
endif()

project(SingleLevelSetWhitaker)

find_package(ITK REQUIRED)
if(ITK_FOUND)
  include(${ITK_USE_FILE})
endif()

find_package(VTK REQUIRED)
if(VTK_FOUND)
  include(${VTK_USE_FILE})
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(SingleLevelSetWhitaker SingleLevelSetWhitaker.cxx)
target_link_libraries(SingleLevelSetWhitaker ${ITK_LIBRARIES} ${VTK_LIBRARIES})

