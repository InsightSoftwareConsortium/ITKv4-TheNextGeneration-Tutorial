project(ITKv4TheNextGenerationTutorialPresentation)

#
#  Copy the tex files to the binary build directory
#
file(GLOB TEX_FILES *.tex)

file(
  COPY  ${TEX_FILES}
  DESTINATION ${ITKv4TheNextGenerationTutorialPresentation_BINARY_DIR}
  )


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


#
#  Copy the image files to the binary build directory
#  to make easier to insert them in the latex document
#
#  This PNG_FILES variable is populated in the Documents/CMakeLists.txt file
#
file(
  COPY  ${PNG_FILES}
  DESTINATION ${ITKv4TheNextGenerationTutorialPresentation_BINARY_DIR}
  )


#
#  Copy the image files to the binary build directory
#  to make easier to insert them in the latex document
#
#  This PDF_FILES variable is populated in the Documents/CMakeLists.txt file
#
file(
  COPY  ${PDF_FILES}
  DESTINATION ${ITKv4TheNextGenerationTutorialPresentation_BINARY_DIR}
  )


#
#  Copy the image files to the binary build directory
#  to make easier to insert them in the latex document
#
#  This JPG_FILES variable is populated in the Documents/CMakeLists.txt file
#
file(
  COPY  ${JPG_FILES}
  DESTINATION ${ITKv4TheNextGenerationTutorialPresentation_BINARY_DIR}
  )



#
# This adds a custom target that generates the output document
# This target depends on the list of copied files created
# with the custom command above and the Plots target.
#
add_custom_target(ShortTutorialPresentation ALL
   COMMAND ${PDFLATEX_COMPILER}
        ${ITKv4TheNextGenerationTutorialPresentation_SOURCE_DIR}/ITKv4TheNextGenerationTutorial.tex
        -output-directory ${ITKv4TheNextGenerationTutorialPresentation_BINARY_DIR}
   COMMAND ${PDFLATEX_COMPILER}
        ${ITKv4TheNextGenerationTutorialPresentation_SOURCE_DIR}/ITKv4TheNextGenerationTutorial.tex
        -output-directory ${ITKv4TheNextGenerationTutorialPresentation_BINARY_DIR}
   DEPENDS ${COPY_RESULTS} ${REPORT_ELEMENTS}
   WORKING_DIRECTORY ${ITKv4TheNextGenerationTutorialPresentation_BINARY_DIR}
   )
