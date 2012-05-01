#
# Here we will add the hands-on exercises
#
find_package(ITK)
cmake_minimum_required(VERSION 2.8)

include(${ITK_USE_FILE})

add_executable(ImageReadWrite ImageReadWrite.cxx )
target_link_libraries(ImageReadWrite ${ITK_LIBRARIES})

add_subdirectory( LevelSets )
add_subdirectory( Registration )
option( USE_OPENCV "Build the OpenCV bridge support." ON )
if( USE_OPENCV )
  add_subdirectory( OpenCVIntroduction )
  add_subdirectory( ITKOpenCVBridge )
endif()
option( USE_VIDEOBRIDGE "Build the OpenCV bridge support." ON )
if( USE_VIDEOBRIDGE )
  add_subdirectory( ITKVideoPipeline )
endif()
