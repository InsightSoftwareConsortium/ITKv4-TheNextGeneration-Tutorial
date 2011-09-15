project(ITKv4TheNextGenerationTutorialPresentation)


#
#  Copy the .cxx files to the binary build directory
#  to make easier to source them as code snippets.
#
#  This CXX_FILES variable is populated in the top CMakeLists.txt file
#
file(
  COPY  ${CXX_FILES}
  DESTINATION ${ITKv4TheNextGenerationTutorialPresentation_BINARY_DIR}
  )


add_latex_document( ITKv4TheNextGenerationTutorial.tex
  INPUTS FEM.tex GPU.tex ITKv4LevelSets.tex Modularization.tex Registration.tex
  IMAGE_DIRS ../Art
  DEFAULT_PDF
  )
