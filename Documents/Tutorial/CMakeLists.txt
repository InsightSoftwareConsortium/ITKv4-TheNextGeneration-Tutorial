project(ITKv4TheNextGenerationTutorialPresentation)

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

