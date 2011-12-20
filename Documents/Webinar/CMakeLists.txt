
project(ITKv4TheNextGenerationWebinar)

#
#  Copy the .cxx files to the binary build directory
#  to make easier to source them as code snippets.
#
#  This CXX_FILES variable is populated in the top CMakeLists.txt file
#
file(
  COPY  ${CXX_FILES}
  DESTINATION ${ITKv4TheNextGenerationWebinar_BINARY_DIR}
  )


add_latex_document( ITK_4_0_0_webinar.tex
  INPUTS Overview.tex Modularization.tex
  IMAGE_DIRS ../Art
  DEFAULT_PDF
  )
