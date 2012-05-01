add_executable( RegistrationExercises RegistrationExercises.cxx )
target_link_libraries( RegistrationExercises ${ITK_LIBRARIES} )

add_executable( Rigid2DRegistration Rigid2DRegistration.cxx )
target_link_libraries( Rigid2DRegistration ${ITK_LIBRARIES} )
