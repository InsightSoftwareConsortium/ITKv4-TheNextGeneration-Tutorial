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
add_subdirectory( OpenCVIntroduction )
add_subdirectory( ITKOpenCVBridge )
add_subdirectory( ITKVideoPipeline )
