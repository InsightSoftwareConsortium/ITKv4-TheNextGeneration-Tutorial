include( Support/UseLATEX.cmake )

option( BUILD_TUTORIAL "Build the tutorial presentation" OFF )
if( BUILD_TUTORIAL )
  add_subdirectory( Tutorial )
endif()

option( BUILD_WEBINAR "Build the webinar presentation" ON )
if( BUILD_WEBINAR )
  add_subdirectory( Webinar )
endif()
