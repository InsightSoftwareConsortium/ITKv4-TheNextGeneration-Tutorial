project(ITKv4TheNextGenerationTutorialPresentation)

add_latex_document( ITKv4TheNextGenerationTutorial.tex
  INPUTS FEM.tex GPU.tex ITKv4LevelSets.tex Modularization.tex Registration.tex
  IMAGE_DIRS ../Art
  DEFAULT_PDF
  )
