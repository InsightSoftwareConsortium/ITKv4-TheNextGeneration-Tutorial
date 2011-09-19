set(DOCUMENTATION "This module contains filters that implement RAT.")

itk_module(ITKRAT
  DEPENDS
    ITKThresholding
    ITKImageGradient
    ITKMathematicalMorphology
  TEST_DEPENDS
    ITKTestKernel
  DESCRIPTION
    "${DOCUMENTATION}"
)
